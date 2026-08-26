#include "SceneRenderer.h"
#include "core/math/Transform.h"
#include "gfx/Camera.h"
#include "gfx/Framebuffer.h"
#include "gfx/ShaderManager.h"
#include "gfx/Texture.h"
#include "gfx/mesh/primitives/Quad.h"
#include "Renderer.h"
#include "core/math/boundingbox/AABB.h"
#include "core/math/volumes/SphereVolume.h"
#include "gfx/mesh/SkeletalMesh.h"
#include "gfx/Pipeline.h"
#include "gfx/Query.h"

namespace BHive
{
#define MAX_OBJECTS 10
#define VISIBILITY_BUFFER_SIZE 16 + 16 * MAX_OBJECTS
#define OBJECT_STRIDE sizeof(ObjectData)
#define OBJECT_BUFFER_SIZE 16 + OBJECT_STRIDE *MAX_OBJECTS
#define DRAWCOMMAND_BUFFER_SIZE MULTI_DRAW_INDIRECT_STRIDE *MAX_OBJECTS
#define MAX_LIGHTS Lights::sMaxLights

	struct alignas(16) ObjectData
	{
		glm::mat4 model{1.0f};						   // model matrix
		glm::vec4 center_radius{0.f, 0.0f, 0.0f, 0.f}; // bounding sphere center.xyz + radius
		uint32_t meshIndex = 0;						   // which mesh this instance belongs to
	};

	struct GPUFrustum
	{
		glm::vec4 planes[6];
	};

	struct RenderBatch
	{
		// vao -> material[submissions]
		std::unordered_map<Ref<VertexArray>, std::unordered_map<ResourceHandle, std::vector<FSubMeshSubmission>>> MaterialBatches;

		std::vector<ObjectData> ObjectDatas;

		std::vector<MultiDrawIndirectCommand> DrawCommands;

		RenderBatch(Ref<FRenderQueue> queue)
			: mQueue(queue)
		{
			ObjectDatas.reserve(MAX_OBJECTS);
			DrawCommands.reserve(MAX_OBJECTS);
			mHandle = mQueue->OnQueueChanged.Add(this, &RenderBatch::OnRenderQueueChanged);
		}

		~RenderBatch() { mQueue->OnQueueChanged.Remove(mHandle); }

		void Build(const SubMeshSubmissions &bucket)
		{
			if (!mNeedsUpdate)
				return;

			MaterialBatches.clear();
			ObjectDatas.clear();
			DrawCommands.clear();

			uint32_t meshIndex = 0;

			for (auto &o : bucket)
			{
				// object data
				auto &ctx = mQueue->ResolveContext(o.Context);
				auto pos = ctx.Transform.GetTranslation();
				auto model = ctx.Transform.ToMat4();
				auto model = ctx.Transform.ToMat4();
				auto radius = o.BoundingBox.GetRadius();
				auto vao = ctx.VAO;
				auto &s = o.SubMesh;

				// submesh data
				auto &group = MaterialBatches[vao];
				auto &submissions = group[o.MaterialHandle];

				auto &submission = submissions.emplace_back(o);
				submission.MeshIndex = meshIndex;

				auto &inst = ObjectDatas.emplace_back();
				inst.center_radius = glm::vec4(pos, radius);
				inst.model = model;
				inst.meshIndex = meshIndex;

				auto &cmd = DrawCommands.emplace_back();
				cmd.indexCount = s.IndexCount;
				cmd.instanceCount = 0; // GPU increments this
				cmd.firstIndex = s.StartIndex;
				cmd.vertexOffset = s.StartVertex;
				cmd.firstInstance = meshIndex; // GPU will use visibleIndices[]

				meshIndex++;
			}

			mNeedsUpdate = false;
		}

		void Draw(FPass &pass, Ref<GeneralBuffer> indirect)
		{
			uint32_t globalOffset = 0;
			// render meshes
			for (auto &[vao, matMap] : MaterialBatches)
			{
				vao->DeclareAccess(pass, EBufferUsage::IndirectRead, EBufferUsage::IndirectRead);

				for (auto &[material, submissions] : matMap)
				{
					if (!material)
						continue;

					pass.Emplace<CmdBindMaterial>()(material.As<Material>());

					uint32_t count = (uint32_t)submissions.size();
					pass.Emplace<CmdMultiDrawIndexedIndirect>()(ETopologyMode::Triangles, indirect.get(), vao.get(), count, MULTI_DRAW_INDIRECT_STRIDE, globalOffset);
					globalOffset += count * MULTI_DRAW_INDIRECT_STRIDE;
				}
			}
		}

		void OnRenderQueueChanged() { mNeedsUpdate = true; }

		uint32_t InstanceCount() const { return static_cast<uint32_t>(ObjectDatas.size()); }

	private:
		Ref<FRenderQueue> mQueue;

		bool mNeedsUpdate = false;

		MultiEventHandle mHandle = 0;
	};

	void SceneRenderer::Init(const glm::uvec2 &size)
	{
		mSize = size;

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

		mFramebuffer = Framebuffer::Create(specs);

		mCameraUBO = GeneralBuffer::Create(sizeof(FView), EBufferType::UniformBuffer);

		for (uint32_t i = 0; i < 2; i++)
		{
			mInstanceDataBuffer[i] = GeneralBuffer::Create(OBJECT_BUFFER_SIZE, EBufferType::StorageBuffer, EBufferLifetime::Dynamic);
			mIndirectDrawBuffer[i] = GeneralBuffer::Create(DRAWCOMMAND_BUFFER_SIZE, EBufferType::StorageBuffer | EBufferType::IndirectBuffer, EBufferLifetime::Dynamic);
			mVisibleBuffer[i] = GeneralBuffer::Create(VISIBILITY_BUFFER_SIZE, EBufferType::StorageBuffer, EBufferLifetime::Dynamic);
			mFrustrumOcclusionMaterial[i] = CreateRef<Material>("FrustumOcclusion.glsl");
		}

		mPostProcessStack.Init(size);
		mLights.Init();
		// mShadows.Init();
	}

	void SceneRenderer::SetEnvironmentTexture(const Ref<Texture2D> &hdr)
	{
		mEnvironment.SetHDR(hdr);
		mEnvironment.Update();
	}

	void SceneRenderer::Begin(const Camera *camera, const glm::mat4 &view)
	{
		auto &renderer = Renderer::Get();

		mView = FView::Create(camera->GetProjection(), view);
		mFrustum.Update(camera->GetProjection(), view);

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
		static std::string pipelineNames[2] = {"MESH_OPAQUE", "MESH_TRANSPARENT"};

		auto &cameraPass = renderer.BeginPass("CameraData", EPassType::OffScreen);
		cameraPass.BeginPhase(EPhaseType::Transfer);
		cameraPass.Emplace<CmdSetBufferData>()(mCameraUBO, &mView, sizeof(FView));
		cameraPass.EndPhase();
		renderer.EndPass();

		GPUFrustum gpuFrustum;
		auto &p = mFrustum.GetPlanes();
		for (uint32_t i = 0; i < 6; i++)
		{
			gpuFrustum.planes[i] = glm::vec4(p[i].Normal, p[i].Distance);
		}

		for (uint32_t i = 0; i < 2; i++)
		{
			mFrustrumOcclusionMaterial[i]->SetParam("frustum", MaterialParam(gpuFrustum));

			auto &batch = *mRenderBatches[i];
			auto instanceCount = batch.InstanceCount();
			auto instanceBuffer = mInstanceDataBuffer[i];
			auto visibilityBuffer = mVisibleBuffer[i];
			auto indirectBuffer = mIndirectDrawBuffer[i];

			auto &batchData = renderer.BeginPass("Set Batch Data", EPassType::OffScreen);
			batchData.BeginPhase(EPhaseType::Transfer);
			batchData.Emplace<CmdClearBuffer>()(visibilityBuffer);
			batchData.Emplace<CmdClearBuffer>()(instanceBuffer);
			batchData.Emplace<CmdClearBuffer>()(indirectBuffer);
			batchData.Emplace<CmdSetBufferData>()(instanceBuffer, &instanceCount, sizeof(uint32_t));
			batchData.Emplace<CmdSetBufferData>()(instanceBuffer, batch.ObjectDatas.data(), sizeof(ObjectData) * instanceCount, 16U);
			batchData.Emplace<CmdSetBufferData>()(indirectBuffer, batch.DrawCommands.data(), sizeof(MultiDrawIndirectCommand) * batch.DrawCommands.size());
			batchData.EndPhase();
			renderer.EndPass();

			// frustum pass

			uint32_t groups = (instanceCount + 256) / 256;
			auto &occlusionPass = renderer.BeginPass("Occlusion " + passNames[i], EPassType::OffScreen);
			occlusionPass.BeginPhase(EPhaseType::Compute);
			occlusionPass.BindGlobal(0, 0, mCameraUBO);
			occlusionPass.BindGlobal(3, 0, instanceBuffer);
			occlusionPass.BindGlobal(3, 1, indirectBuffer);
			occlusionPass.BindGlobal(3, 2, visibilityBuffer);

			occlusionPass.UseBuffer(indirectBuffer, EBufferUsage::StorageWrite);
			occlusionPass.UseBuffer(visibilityBuffer, EBufferUsage::StorageWrite);
			occlusionPass.UseBuffer(instanceBuffer, EBufferUsage::StorageRead);
			occlusionPass.UseBuffer(mCameraUBO, EBufferUsage::UniformRead);
			occlusionPass.Emplace<CmdBindMaterial>()(mFrustrumOcclusionMaterial[i].get());
			occlusionPass.Emplace<CmdDispatch>()(groups, 1, 1);
			occlusionPass.EndPhase();
			renderer.EndPass();

			// render scene passes
			auto &pass = renderer.BeginPass("Scene " + passNames[i], EPassType::OffScreen, states[i]);
			pass.BeginPhase("Phase " + passNames[i], EPhaseType::Graphics);
			pass.BindGlobal(0, 0, mCameraUBO);
			pass.BindGlobal(0, 1, mLights.GetBuffer());
			pass.BindGlobal(0, 2, brdfLUT);
			pass.BindGlobal(0, 3, prefilter);
			pass.BindGlobal(0, 4, irradiance);
			pass.BindGlobal(3, 0, instanceBuffer);
			pass.BindGlobal(3, 2, visibilityBuffer);
			pass.UseFramebuffer(mFramebuffer);
			pass.UseTexture(prefilter, EImageUsage::ColorRead);
			pass.UseTexture(irradiance, EImageUsage::ColorRead);
			pass.UseTexture(brdfLUT, EImageUsage::ColorRead);
			pass.UseBuffer(mCameraUBO, EBufferUsage::UniformRead);
			pass.UseBuffer(mLights.GetBuffer(), EBufferUsage::StorageRead);
			pass.UseBuffer(instanceBuffer, EBufferUsage::StorageRead);
			pass.UseBuffer(visibilityBuffer, EBufferUsage::StorageRead);

			pass.Emplace<CmdBindPipeline>()(PipelineRegistry::Get(pipelineNames[i]));
			batch.Draw(pass, indirectBuffer);
			pass.EndPhase();

			renderer.EndPass();
		}

		auto &linePass = renderer.BeginPass("Line Renderer", EPassType::OffScreen, states[1]);

		linePass.BeginPhase("Line Rendering", EPhaseType::Graphics);
		linePass.BindGlobal(0, 0, mCameraUBO);
		linePass.UseFramebuffer(mFramebuffer);
		linePass.UseBuffer(mCameraUBO, EBufferUsage::UniformRead);
		renderer.EndBatching();
		linePass.EndPhase();
		renderer.EndPass();

		auto &transitionPass = renderer.BeginPass("Transition to read", EPassType::OffScreen, {});
		transitionPass.BeginPhase("Transition to read", EPhaseType::Transfer);
		transitionPass.UseTexture(mFramebuffer->GetColorAttachment(), EImageUsage::ColorRead);
		transitionPass.EndPhase();
		renderer.EndPass();

		// post process
		FPostProcessTextureSet set{mFramebuffer->GetColorAttachment(), mFramebuffer->GetDepthAttachment()};
		mOutputTexture = mPostProcessStack.Build(renderer.GetActiveGraph(), set);
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

	void SceneRenderer::UpdateMesh(ContextHandle requestHandle, Ref<BaseMesh> mesh)
	{
		mRenderQueue->RemoveSubmissionsForContext(requestHandle);

		if (!mesh)
			return;

		mRenderQueue->AddSubmissionsForMesh(requestHandle, mesh);

		mRenderQueue->OnQueueChanged.Broadcast();
	}

	float SceneRenderer::GetDistanceToCamera(const FTransform &transform)
	{
		const auto &C = mView.Position;
		return glm::distance(glm::vec3(C), transform[2]);
	}

	void SceneRenderer::Resize(const glm::uvec2 &size)
	{
		mSize = size;

		mFramebuffer->Resize(size);

		mPostProcessStack.Init(size);
	}

	void SceneRenderer::RenderToScreen()
	{
		mFramebuffer->BlitToWindow(0, 0, mSize.x, mSize.y);
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