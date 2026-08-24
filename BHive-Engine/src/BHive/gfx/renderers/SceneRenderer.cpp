#include "core/math/Transform.h"
#include "gfx/Camera.h"
#include "gfx/Framebuffer.h"
#include "gfx/ShaderManager.h"
#include "gfx/Texture.h"
#include "gfx/mesh/primitives/Quad.h"
#include "gfx/renderers/Renderer.h"
#include "SceneRenderer.h"
#include "core/math/boundingbox/AABB.h"
#include "core/math/volumes/SphereVolume.h"
#include "gfx/mesh/SkeletalMesh.h"
#include "gfx/Pipeline.h"
#include "gfx/Query.h"
#include "core/delegates/MultiEventDelegate.h"

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

	DECLARE_MULTI_EVENT(FRenderQueueChanged)

	struct FRenderQueue
	{
		std::vector<FMeshSubmissionContext> Contexts;
		std::vector<Ref<Material>> Materials;
		SubMeshSubmissions OpaqueSubmissions;
		SubMeshSubmissions TransparentSubmissions;
		SubMeshSubmissions ShadowPassRenderData;
		SubMeshSubmissions RenderPassRenderData;

		void Init() { Contexts.reserve(MAX_OBJECTS); }

		uint32_t AddSubmissionContext()
		{
			Contexts.emplace_back();
			return Contexts.size() - 1;
		}

		uint32_t AddMaterial(Ref<Material> mat)
		{
			auto it = std::find(Materials.begin(), Materials.end(), mat);
			if (it != Materials.end())
				return std::distance(Materials.begin(), it);

			Materials.emplace_back(mat);
			return Materials.size() - 1;
		}

		void AddRequest(const FMeshSubmissionRequest &request) { mPendingRequests.emplace_back(request); }

		void AddSubmission(FSubMeshSubmission &submission)
		{
			auto flags = submission.BitFlags;
			auto &submissions = flags[0] ? TransparentSubmissions : OpaqueSubmissions;
			submissions.emplace_back(submission);

			if (flags[1])
				ShadowPassRenderData.emplace_back(submission);
		}

		void Rebuild()
		{
			for (auto &r : mPendingRequests)
			{
				const auto &materials = r.Materials;
				const auto &subMeshes = r.Mesh->GetSubMeshes();
				const auto subMeshCount = subMeshes.size();

				if (subMeshCount == 0)
				{
					return;
				}

				auto ctxIndex = AddSubmissionContext();
				auto &ctx = Contexts.at(ctxIndex);
				ctx.Transform = r.Transform;
				ctx.EntityID = r.EntityID;
				ctx.InstanceTransforms = r.InstanceTransforms;
				ctx.BoneTransforms = r.BoneTransforms;
				ctx.VAO = r.Mesh->GetVertexArray();

				for (auto &s : subMeshes)
				{
					auto material = materials.get_material(s.MaterialIndex);
					if (!material)
						return;

					FSubMeshSubmission submission{};
					submission.ContextIndex = ctxIndex;
					submission.SubMesh = s;
					submission.BoundingBox = r.Mesh->GetBoundingBox();
					submission.BitFlags[0] = material->IsTransparent();
					submission.BitFlags[1] = material->ShouldCastShadows();
					submission.MaterialIndex = AddMaterial(material);
					AddSubmission(submission);
				}
			}
		}

		void BuildQueue(const glm::mat4 &view)
		{ // back-to-front
			static auto sorting = [=](FSubMeshSubmission &a, FSubMeshSubmission &b)
			{
				auto aCtx = Contexts.at(a.ContextIndex);
				auto bCtx = Contexts.at(b.ContextIndex);

				float za = (view * glm::vec4(aCtx.Transform.GetTranslation(), 1.0f)).z;
				float zb = (view * glm::vec4(bCtx.Transform.GetTranslation(), 1.0f)).z;
				return za > zb;
			};

			auto requestCount = mPendingRequests.size();
			if (requestCount)
			{
				LOG_TRACE("Building Pending Mesh Submissions {}", requestCount)
				Contexts.clear();
				OpaqueSubmissions.clear();
				TransparentSubmissions.clear();
				ShadowPassRenderData.clear();
				RenderPassRenderData.clear();

				Rebuild();

				std::sort(TransparentSubmissions.begin(), TransparentSubmissions.end(), sorting);

				mPendingRequests.clear();

				OnQueueChanged.Broadcast();
			}
		}

		Ref<Material> GetMaterial(uint32_t index) { return Materials.at(index); }

		FRenderQueueChangedEvent OnQueueChanged;

	private:
		std::vector<FMeshSubmissionRequest> mPendingRequests;
	};

	struct RenderBatch
	{
		std::unordered_map<uint32_t, std::unordered_map<Ref<VertexArray>, std::vector<FSubMeshSubmission>>> MaterialBatches;

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
			LOG_TRACE("Batch NeedsUpdate {}", mNeedsUpdate);

			if (!mNeedsUpdate)
				return;

			MaterialBatches.clear();
			ObjectDatas.clear();
			DrawCommands.clear();

			uint32_t meshIndex = 0;

			for (auto &o : bucket)
			{
				// object data
				auto &ctx = mQueue->Contexts.at(o.ContextIndex);
				auto pos = ctx.Transform.GetTranslation();
				auto model = ctx.Transform.ToMat4();
				auto radius = o.BoundingBox.GetRadius();
				auto vao = ctx.VAO;
				auto &s = o.SubMesh;

				// submesh data

				auto &group = MaterialBatches[o.MaterialIndex];
				auto &submissions = group[vao];

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
			LOG_TRACE("Batch Updated!");
		}

		void Draw(FPass &pass, Ref<GeneralBuffer> indirect)
		{

			LOG_TRACE("DrawCommand Count {} for pass {}", DrawCommands.size(), pass.Name);

			uint32_t globalOffset = 0;
			// render meshes
			for (auto &[mat, vaoMap] : MaterialBatches)
			{
				auto material = mQueue->GetMaterial(mat);

				pass.Emplace<CmdBindMaterial>()(material.get());

				for (auto &[vao, submissions] : vaoMap)
				{
					uint32_t count = (uint32_t)submissions.size();

					vao->DeclareAccess(pass, EBufferUsage::IndirectRead, EBufferUsage::IndirectRead);
					pass.Emplace<CmdMultiDrawIndexedIndirect>()(ETopologyMode::Triangles, indirect.get(), vao.get(), count, MULTI_DRAW_INDIRECT_STRIDE, globalOffset);
					globalOffset += count * MULTI_DRAW_INDIRECT_STRIDE;
				}
			}
		}

		void OnRenderQueueChanged()
		{
			mNeedsUpdate = true;
			LOG_TRACE("Rendere Queue Changed {}", mNeedsUpdate);
		}

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
		mRenderQueue->Init();

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
		}

		mPostProcessStack.Init(size);
		mLights.Init();
		// mShadows.Init();

		mFrustrumOcclusionMaterial = CreateRef<Material>("FrustumOcclusion.glsl");
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

		mRenderBatches[0]->Build(mRenderQueue->OpaqueSubmissions);
		mRenderBatches[1]->Build(mRenderQueue->TransparentSubmissions);

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

		for (uint32_t i = 0; i < 1; i++)
		{
			mFrustrumOcclusionMaterial->SetParam("frustum", MaterialParam(mFrustum.GetPlanes()));

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
			occlusionPass.BindGlobal(0, 0, mCameraUBO);
			occlusionPass.BindGlobal(3, 0, instanceBuffer);
			occlusionPass.BindGlobal(3, 1, indirectBuffer);
			occlusionPass.BindGlobal(3, 2, visibilityBuffer);

			occlusionPass.BeginPhase(EPhaseType::Compute);
			occlusionPass.UseBuffer(indirectBuffer, EBufferUsage::StorageWrite);
			occlusionPass.UseBuffer(visibilityBuffer, EBufferUsage::StorageWrite);
			occlusionPass.UseBuffer(instanceBuffer, EBufferUsage::StorageRead);
			occlusionPass.UseBuffer(mCameraUBO, EBufferUsage::UniformRead);
			occlusionPass.Emplace<CmdBindMaterial>()(mFrustrumOcclusionMaterial.get());
			occlusionPass.Emplace<CmdDispatch>()(groups, 1, 1);
			occlusionPass.EndPhase();
			renderer.EndPass();

			// render scene passes
			auto &pass = renderer.BeginPass("Scene " + passNames[i], EPassType::OffScreen, states[i]);
			// global buffers
			pass.BindGlobal(0, 0, mCameraUBO);
			pass.BindGlobal(0, 1, mLights.GetBuffer());
			pass.BindGlobal(0, 2, brdfLUT);
			pass.BindGlobal(0, 3, prefilter);
			pass.BindGlobal(0, 4, irradiance);
			pass.BindGlobal(3, 0, instanceBuffer);
			pass.BindGlobal(3, 2, visibilityBuffer);

			pass.BeginPhase("Phase " + passNames[i], EPhaseType::Graphics);
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

	void SceneRenderer::SubmitMesh(const FMeshSubmissionRequest &info)
	{
		mRenderQueue->AddRequest(info);
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