#include "SceneRenderer.h"
#include "Renderer.h"
#include "core/math/Transform.h"
#include "core/math/boundingbox/AABB.h"
#include "core/math/volumes/SphereVolume.h"
#include "gfx/Camera.h"
#include "gfx/Query.h"
#include "gfx/ShaderManager.h"
#include "gfx/Texture.h"
#include "gfx/factories/GFXFactories.h"
#include "gfx/mesh/SkeletalMesh.h"

// {
// 	auto state = Pipeline::GetDefaultGraphicsPipelineState();
// 	state.Blend.Enabled = false;
// 	state.Depth.DepthTest = true;
// 	state.Depth.DepthWrite = true;
// 	Register("MESH_MASKED", state);
// }

// {
// 	auto state = Pipeline::GetDefaultGraphicsPipelineState();
// 	state.Depth.DepthTest = true;
// 	state.Depth.DepthWrite = true;
// 	Register("MESH_SHADOW", state);
// }

namespace BHive
{
#define MULTI_DRAW_INDIRECT_STRIDE sizeof(MultiDrawIndirectCommand)
#define MAX_OBJECTS 100
#define VISIBILITY_BUFFER_SIZE 16 + 16 * MAX_OBJECTS
#define OBJECT_STRIDE sizeof(ObjectData)
#define OBJECT_BUFFER_SIZE 16 + OBJECT_STRIDE *MAX_OBJECTS
#define DRAWCOMMAND_BUFFER_SIZE MULTI_DRAW_INDIRECT_STRIDE *MAX_OBJECTS
#define MAX_LIGHTS Lights::sMaxLights
#define MAX_BONES 200
#define BONE_BUFFER_SIZE sizeof(glm::mat4) * MAX_BONES *MAX_OBJECTS

	struct MultiDrawIndirectCommand
	{
		uint32_t indexCount = 0;
		uint32_t instanceCount = 0;
		uint32_t firstIndex = 0;
		int32_t vertexOffset = 0;
		uint32_t firstInstance = 0;
	};

	struct alignas(16) ObjectData
	{
		glm::mat4 ModelMatrix{1.0f};				  // model matrix
		glm::vec4 CenterRadius{0.f, 0.0f, 0.0f, 0.f}; // bounding sphere center.xyz + radius
		uint32_t ID = 0;							  // which mesh this instance belongs to
		uint32_t boneOffset = 0;					  // bone offset
		glm::vec4 debugColor;
	};

	struct RenderBatch
	{
		struct DrawBatch
		{
			uint32_t FirstCommand = 0;
			uint32_t CommandCount = 0;
		};

		// vao -> material[submissions]
		std::unordered_map<VertexArrayPtr, std::unordered_map<MaterialPtr, DrawBatch>> MaterialBatches;

		std::vector<ObjectData> ObjectDatas;

		std::vector<glm::mat4> BoneDatas;

		std::vector<MultiDrawIndirectCommand> DrawCommands;

		RenderBatch(Ref<FRenderQueue> queue)
			: mQueue(queue)
		{
			ObjectDatas.reserve(MAX_OBJECTS);
			DrawCommands.reserve(MAX_OBJECTS);
			BoneDatas.reserve(200 * MAX_OBJECTS);
		}

		void Build(const SubMeshSubmissions &bucket)
		{
			MaterialBatches.clear();
			ObjectDatas.clear();
			DrawCommands.clear();
			BoneDatas.clear();

			uint32_t boneOffset = 0;
			for (auto &o : bucket)
			{
				// object data
				auto &ctx = mQueue->ResolveContext(o.Context);
				auto pos = ctx.Transform.GetTranslation();
				auto model = ctx.Transform.ToMat4();

				auto vao = ctx.VAO;
				auto &s = o.SubMesh;
				auto radius = s.Bounds.GetRadius();

				auto objectID = ObjectDatas.size();

				auto &inst = ObjectDatas.emplace_back();
				inst.CenterRadius = glm::vec4(pos, radius); //<= change to local submesb pos
				inst.ModelMatrix = model * s.Transformation;
				inst.ID = objectID;

				// submesh data
				auto &group = MaterialBatches[vao];
				auto &batch = group[o.Material];

				if (batch.CommandCount == 0)
					batch.FirstCommand = (uint32_t)DrawCommands.size();

				batch.CommandCount++;

				// bones
				auto &bones = ctx.BoneTransforms;
				if (auto count = bones.size())
				{
					BoneDatas.insert(BoneDatas.end(), bones.begin(), bones.end());
					inst.boneOffset = boneOffset;
					boneOffset += count;
				}

				auto &cmd = DrawCommands.emplace_back();
				cmd.indexCount = s.IndexCount;
				cmd.instanceCount = 0; // GPU increments this
				cmd.firstIndex = s.StartIndex;
				cmd.vertexOffset = s.StartVertex;
				cmd.firstInstance = UINT32_MAX; // GPU will use visibleIndices[]
			}
		}

		void Draw(FPass &pass, BufferPtr indirect)
		{
			// render meshes
			for (auto &[vao, matMap] : MaterialBatches)
			{
				auto v = vao.As<VertexArray>();
				v->DeclareAccess(pass, EBufferUsage::IndirectRead, EBufferUsage::IndirectRead);

				for (auto &[material, batch] : matMap)
				{
					if (!material)
						continue;

					pass.Emplace<CmdBindMaterial>()(material.As<Material>());

					uint32_t offset = batch.FirstCommand * sizeof(MultiDrawIndirectCommand);

					pass.Emplace<CmdMultiDrawIndexedIndirect>()(ETopologyMode::Triangles, indirect, vao, batch.CommandCount, MULTI_DRAW_INDIRECT_STRIDE, offset);
				}
			}
		}

		uint32_t InstanceCount() const { return static_cast<uint32_t>(ObjectDatas.size()); }

	private:
		Ref<FRenderQueue> mQueue;
	};

	void SceneRenderer::Init(const glm::uvec2 &size)
	{
		mSize = size;

		InitResourceSets();

		mRenderQueue = CreateRef<FRenderQueue>();
		mRenderQueue->Init(MAX_OBJECTS);

		mRenderBatches.resize(2);
		mRenderBatches[0] = CreateRef<RenderBatch>(mRenderQueue);
		mRenderBatches[1] = CreateRef<RenderBatch>(mRenderQueue);

		// Initialize the framebuffer or any other resources needed for rendering
		FramebufferSpecification specs;
		specs.Size = size;
		specs.Attachments.AddColorAttachment({FTextureCreateInfo{.Format = EFormat::RGBA32F, .WrapMode = EWrapMode::CLAMP_TO_EDGE}});
		specs.Attachments.SetDepthAttachment({FTextureCreateInfo{.Format = EFormat::DEPTH24_STENCIL8, .WrapMode = EWrapMode::CLAMP_TO_EDGE}});
		specs.DebugName = "SceneRenderer";

		mFramebuffer = FramebufferFactory::Create(specs);

		mCameraUBO = BufferFactory::Create(sizeof(SceneView), EBufferType::UniformBuffer);

		for (uint32_t i = 0; i < 2; i++)
		{
			mInstanceDataBuffer[i] = BufferFactory::Create(OBJECT_BUFFER_SIZE, EBufferType::StorageBuffer, EBufferLifetime::Dynamic);
			mIndirectDrawBuffer[i] = BufferFactory::Create(DRAWCOMMAND_BUFFER_SIZE, EBufferType::StorageBuffer | EBufferType::IndirectBuffer, EBufferLifetime::Dynamic);
			mVisibleBuffer[i] = BufferFactory::Create(VISIBILITY_BUFFER_SIZE, EBufferType::StorageBuffer, EBufferLifetime::Dynamic);
			mFrustrumOcclusionMaterial[i] = MaterialFactory::Create("FrustumOcclusion.glsl");
			mBoneBuffer[i] = BufferFactory::Create(BONE_BUFFER_SIZE, EBufferType::StorageBuffer, EBufferLifetime::Dynamic);
		}
		mFrustumMaterial = MaterialFactory::Create("Frustum.glsl");

		mPostProcessStack.Init(size);
		mLights.Init();

		InitPipelines();

		BindResourceSets();
	}

	void SceneRenderer::SetEnvironmentTexture(Texture2DPtr hdr)
	{
		mEnvironment.SetHDR(hdr);
		mEnvironment.Update();

		auto envMaps = mEnvironment.GetCurrentMaps();
		auto brdfLut = mEnvironment.GetBRDFLUT();

		auto globalSet = mSceneSets.GlobalSet.As<ResourceSet>();
		globalSet->SetTexture(2, brdfLut);
		globalSet->SetTexture(3, envMaps.PreFilter);
		globalSet->SetTexture(4, envMaps.Irradiance);
	}

	void SceneRenderer::Begin(const Camera *camera, const glm::mat4 &view)
	{
		auto &renderer = Renderer::Get();

		mSceneView.View = FView::Create(camera->GetProjection(), view);
		mSceneView.Frustum = Frustum(camera->GetProjection(), view);

		renderer.BeginBatching();
		mLights.BeginRecording();
		// mShadows.BeginRecording();

		mRenderQueue->BuildQueue(view);
	}

	void SceneRenderer::End()
	{
		mLights.EndRecording();
		// mShadows.EndRecording();

		mRenderBatches[0]->Build(mRenderQueue->Opaque);
		mRenderBatches[1]->Build(mRenderQueue->Transparent);

		auto &renderer = Renderer::Get();

		FPassState states[2];
		states[0].Color = {EAttachmentLoadState::Clear, EAttachmentStoreState::Store, {0.15f, 0.15f, 0.15f, 1.0f}};
		states[0].Depth = {EAttachmentLoadState::Clear, EAttachmentStoreState::Store};

		states[1].Color = {EAttachmentLoadState::Load, EAttachmentStoreState::Store, {0.1f, 0.1f, 0.1f, 1.0f}};
		states[1].Depth = {EAttachmentLoadState::Load, EAttachmentStoreState::Store};

		auto environmentMaps = mEnvironment.GetCurrentMaps();
		auto prefilter = environmentMaps.PreFilter;
		auto irradiance = environmentMaps.Irradiance;
		auto brdfLUT = mEnvironment.GetBRDFLUT();

		static std::string passNames[2] = {"OpaquePass", "TransparentPass"};
		auto &cameraPass = renderer.BeginPass("CameraData", EPassType::OffScreen);
		cameraPass.BeginPhase(EPhaseType::Transfer);
		cameraPass.Emplace<CmdSetBufferData>()(mCameraUBO, &mSceneView, sizeof(SceneView));
		cameraPass.EndPhase();
		renderer.EndPass();

		PipelinePtr pipelines[2] = {mOpaquePipeline, mTransparentPipeline};
		ResourceSetPtr objectSets[2] = {mSceneSets.OpaqueObjectSet, mSceneSets.TransparentObjectSet};

		for (uint32_t i = 0; i < 1; i++)
		{
			auto &batch = *mRenderBatches[i];
			auto instanceCount = batch.InstanceCount();
			auto instanceBuffer = mInstanceDataBuffer[i];
			auto visibilityBuffer = mVisibleBuffer[i];
			auto indirectBuffer = mIndirectDrawBuffer[i];
			auto boneBuffer = mBoneBuffer[i];

			auto &batchData = renderer.BeginPass("Set Batch Data", EPassType::OffScreen);
			batchData.BeginPhase(EPhaseType::Transfer);
			batchData.Emplace<CmdClearBuffer>()(visibilityBuffer);
			batchData.Emplace<CmdClearBuffer>()(instanceBuffer);
			batchData.Emplace<CmdClearBuffer>()(indirectBuffer);
			batchData.Emplace<CmdSetBufferData>()(instanceBuffer, &instanceCount, sizeof(uint32_t));
			batchData.Emplace<CmdSetBufferData>()(instanceBuffer, batch.ObjectDatas.data(), sizeof(ObjectData) * instanceCount, 16U);
			batchData.Emplace<CmdSetBufferData>()(indirectBuffer, batch.DrawCommands.data(), sizeof(MultiDrawIndirectCommand) * batch.DrawCommands.size());
			batchData.Emplace<CmdSetBufferData>()(boneBuffer, batch.BoneDatas.data(), sizeof(glm::mat4) * batch.BoneDatas.size());
			batchData.EndPhase();
			renderer.EndPass();

			auto objectSet = objectSets[i];
			// frustum pass

			uint32_t groups = (instanceCount + 256) / 256;
			auto &occlusionPass = renderer.BeginPass("Occlusion " + passNames[i], EPassType::OffScreen);
			occlusionPass.BeginPhase(EPhaseType::Compute);
			occlusionPass.BindResourceSet(mSceneSets.GlobalSet);
			occlusionPass.BindResourceSet(objectSet);
			occlusionPass.UseBuffer(indirectBuffer, EBufferUsage::StorageWrite);
			occlusionPass.UseBuffer(visibilityBuffer, EBufferUsage::StorageWrite);
			occlusionPass.UseBuffer(instanceBuffer, EBufferUsage::StorageRead);
			occlusionPass.UseBuffer(mCameraUBO, EBufferUsage::UniformRead);
			occlusionPass.Emplace<CmdBindMaterial>()(mFrustrumOcclusionMaterial[i].As<Material>());
			occlusionPass.Emplace<CmdDispatch>()(groups, 1, 1);
			occlusionPass.EndPhase();
			renderer.EndPass();

			// render scene passes
			auto &pass = renderer.BeginPass("Scene " + passNames[i], EPassType::OffScreen, states[i]);
			pass.BeginPhase("Phase " + passNames[i], EPhaseType::Graphics);
			pass.BindResourceSet(mSceneSets.GlobalSet);
			pass.BindResourceSet(objectSet);
			pass.UseFramebuffer(mFramebuffer);
			pass.UseTexture(prefilter, EImageUsage::ColorRead);
			pass.UseTexture(irradiance, EImageUsage::ColorRead);
			pass.UseTexture(brdfLUT, EImageUsage::ColorRead);
			pass.UseBuffer(mCameraUBO, EBufferUsage::UniformRead);
			pass.UseBuffer(mLights.GetBuffer(), EBufferUsage::StorageRead);
			pass.UseBuffer(instanceBuffer, EBufferUsage::StorageRead);
			pass.UseBuffer(visibilityBuffer, EBufferUsage::StorageRead);

			pass.Emplace<CmdBindPipeline>()(pipelines[i]);
			batch.Draw(pass, indirectBuffer);
			pass.EndPhase();

			renderer.EndPass();
		}

		auto &pass = renderer.BeginPass("Frustum", EPassType::OffScreen, states[1]);
		pass.BeginPhase(EPhaseType::Graphics);
		pass.BindResourceSet(mSceneSets.GlobalSet);
		/*	pass.BindBuffer(0, 0, mCameraUBO);
			pass.BindBuffer(0, 1, mFrustumUBO);*/
		pass.UseFramebuffer(mFramebuffer);
		pass.Emplace<CmdBindPipeline>()(pipelines[0]);
		pass.Emplace<CmdBindMaterial>()(mFrustumMaterial.As<Material>());
		pass.Emplace<CmdSetLineWidth>()(1.0f);
		pass.Emplace<CmdDraw>()(ETopologyMode::Lines, {}, 24);
		pass.EndPhase();
		renderer.EndPass();

		auto &linePass = renderer.BeginPass("Line Renderer", EPassType::OffScreen, states[1]);

		linePass.BeginPhase("Line Rendering", EPhaseType::Graphics);
		linePass.BindResourceSet(mSceneSets.GlobalSet);
		// linePass.BindBuffer(0, 0, mCameraUBO);
		linePass.UseFramebuffer(mFramebuffer);
		linePass.UseBuffer(mCameraUBO, EBufferUsage::UniformRead);
		renderer.EndBatching();
		linePass.EndPhase();
		renderer.EndPass();

		auto framebuffer = mFramebuffer.As<Framebuffer>();

		auto &transitionPass = renderer.BeginPass("Transition to read", EPassType::OffScreen, {});
		transitionPass.BeginPhase("Transition to read", EPhaseType::Transfer);
		transitionPass.UseTexture(framebuffer->GetColorAttachment(), EImageUsage::ColorRead);
		transitionPass.EndPhase();
		renderer.EndPass();

		// post process

		FPostProcessTextureSet set;
		set.SceneColor = framebuffer->GetColorAttachment();
		set.SceneDepth = framebuffer->GetDepthAttachment();
		set.PrevOutput = framebuffer->GetColorAttachment();
		mOutputTexture = mPostProcessStack.Build(renderer.GetActiveGraph(), set);
	}

	void SceneRenderer::SetViewOverride(const FView &view)
	{
		mSceneView.View = view;
	}

	void SceneRenderer::Submit(const DirectionalLight &light)
	{
		mLights.Submit(light);

		// FShadowCascadedCreateInfo shadow_info{};
		// shadow_info.LightDirection = light.GetDirection();
		// shadow_info.CameraProj = mView.Projection;
		// shadow_info.InverseCameraView = mView.View;
		// shadow_info.CameraNearFar = mView.NearFar;
		// shadow_info.LightCascadeFrustumNear = 1.0f;

		// mShadows.SubmitDirectionalLight(shadow_info);
	}

	void SceneRenderer::Submit(const PointLight &light)
	{
		mLights.Submit(light);

		// FShadowCubeCreateInfo shadow_info{};
		// shadow_info.LightPosition = light.GetPosition();
		// shadow_info.LightNearFar = {1.0f, light.GetRadius()};

		// mShadows.SubmitPointLight(shadow_info);
	}

	void SceneRenderer::Submit(const SpotLight &light)
	{
		/*auto inner = glm::cos(glm::radians(info.InnerCutoff));
		auto outer = glm::cos(glm::radians(info.OuterCutoff));*/
		mLights.Submit(light);

		// FShadowFrustumCreateInfo shadow_info{};
		// shadow_info.LightDirection = light.GetDirection();
		// shadow_info.LightPosition = light.GetPosition();

		// // TODO : maybe radius
		// shadow_info.LightAngleNearFar = {glm::radians(light.GetOuterAngleDegrees()), .1f, light.GetRadius()};

		// mShadows.SubmitSpotLight(shadow_info);
	}

	void SceneRenderer::SubmitMesh(const FMeshSubmissionRequest &info, ContextHandle &outHandle)
	{
		outHandle = mRenderQueue->AddMesh(info);
	}

	void SceneRenderer::SubmitMesh(const FMeshSubmissionRequest &info)
	{
		mRenderQueue->AddMesh(info);
	}

	void SceneRenderer::UpdateTransform(ContextHandle requestHandle, const FTransform &t)
	{
		if (!mRenderQueue->IsHandleValid(requestHandle))
			return;

		auto &ctx = mRenderQueue->ResolveContext(requestHandle);
		ctx.Transform = t;

		mRenderQueue->OnQueueChanged.Broadcast();
	}

	void SceneRenderer::UpdateMesh(ContextHandle requestHandle, MeshPtr mesh)
	{
		mRenderQueue->RemoveSubmissionsForContext(requestHandle);

		if (!mesh)
			return;

		mRenderQueue->AddSubmissionsForMesh(requestHandle, mesh);

		mRenderQueue->OnQueueChanged.Broadcast();
	}

	void SceneRenderer::InitPipelines()
	{
		auto opaqueState = Pipeline::GetDefaultGraphicsPipelineState();
		opaqueState.Blend.Enabled = false;
		opaqueState.Depth.DepthTest = true;
		opaqueState.Depth.DepthWrite = true;

		mOpaquePipeline = PipelineFactory::Create(opaqueState);

		auto transparentState = Pipeline::GetDefaultGraphicsPipelineState();
		transparentState.Blend.Enabled = true;
		transparentState.Depth.DepthTest = true;
		transparentState.Depth.DepthWrite = false;

		mTransparentPipeline = PipelineFactory::Create(transparentState);
	}

	void SceneRenderer::InitResourceSets()
	{
		mSceneSets.GlobalSet = ResourceSetFactory::Create(RendererTemplates::Global());
		mSceneSets.OpaqueObjectSet = ResourceSetFactory::Create(RendererTemplates::Object());
		mSceneSets.TransparentObjectSet = ResourceSetFactory::Create(RendererTemplates::Object());
	}

	void SceneRenderer::BindResourceSets()
	{
		auto global = mSceneSets.GlobalSet.As<ResourceSet>();
		global->SetBuffer(0, mCameraUBO);
		global->SetBuffer(1, mLights.GetBuffer());

		auto opaqueSet = mSceneSets.OpaqueObjectSet.As<ResourceSet>();
		opaqueSet->SetBuffer(0, mInstanceDataBuffer[0]);
		opaqueSet->SetBuffer(1, mIndirectDrawBuffer[0]);
		opaqueSet->SetBuffer(2, mVisibleBuffer[0]);
		opaqueSet->SetBuffer(3, mBoneBuffer[0]);

		auto transparentSet = mSceneSets.TransparentObjectSet.As<ResourceSet>();
		transparentSet->SetBuffer(0, mInstanceDataBuffer[1]);
		transparentSet->SetBuffer(1, mIndirectDrawBuffer[1]);
		transparentSet->SetBuffer(2, mVisibleBuffer[1]);
		transparentSet->SetBuffer(3, mBoneBuffer[1]);
	}

	void SceneRenderer::Resize(const glm::uvec2 &size)
	{
		mSize = size;

		mFramebuffer.As<Framebuffer>()->Resize(size);

		mPostProcessStack.Init(size);
	}

	void SceneRenderer::RenderToScreen()
	{
		mFramebuffer.As<Framebuffer>()->BlitToWindow(0, 0, mSize.x, mSize.y);
	}

	void SceneRenderer::AddPostProcessMaterial(const Ref<PostProcessMaterial> &mat)
	{
		mPostProcessStack.Add(mat);
	}

	void SceneRenderer::RemovePostProcessMaterial(const std::string &name)
	{
		// std::erase_if(mPostProcessStack.Materials, [name](auto &e) { return e->GetName() == name; });
	}

	void SceneRenderer::ClearPostProcessEffects()
	{
		// mPostProcessStack.Materials.clear();
	}
} // namespace BHive