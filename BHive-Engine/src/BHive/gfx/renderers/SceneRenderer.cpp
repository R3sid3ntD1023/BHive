#include "core/math/Transform.h"
#include "gfx/Camera.h"
#include "gfx/Framebuffer.h"
#include "gfx/ShaderManager.h"
#include "gfx/Texture.h"
#include "gfx/mesh/primitives/Quad.h"
#include "gfx/renderers/Renderer.h"
#include "SceneRenderer.h"
#include "ShadowRenderer.h"
#include "Lights.h"
#include "core/math/boundingbox/AABB.h"
#include "core/math/volumes/SphereVolume.h"
#include "gfx/mesh/SkeletalMesh.h"
#include "gfx/Pipeline.h"
#include "gfx/Query.h"

namespace BHive
{
	struct FSceneRenderData
	{
		std::unordered_map<Ref<Material>, FMeshRenderDatas> OpaqueData;
		std::unordered_map<Ref<Material>, FMeshRenderDatas> TransparentData;
		FMeshRenderDatas ShadowPassRenderData;
		FMeshRenderDatas RenderPassRenderData;

		ShadowRenderer ShadowRenderer;

		void Init() { ShadowRenderer.Init(MAX_LIGHTS); }

		void Reset()
		{
			OpaqueData.clear();
			TransparentData.clear();
			ShadowPassRenderData.clear();
			RenderPassRenderData.clear();
		}
	};

	struct FDrawRange
	{
		uint32_t First = 0;
		uint32_t Count = 0;
	};

	struct RenderBatch
	{
		std::vector<MultiDrawIndirectCommand> DrawCommands;

		std::unordered_map<Ref<Material>, std::unordered_map<Ref<VertexArray>, FDrawRange>> DrawRanges;

		std::vector<FPerObjectData> ObjectData;

		void Build(const std::unordered_map<Ref<Material>, FMeshRenderDatas> &bucket, uint32_t startBaseInstance = 0)
		{
			DrawCommands.clear();
			DrawRanges.clear();
			ObjectData.clear();

			for (auto &[mat, objects] : bucket)
			{
				for (auto &obj : objects)
				{

					auto &s = obj->SubMesh;
					auto &vao = obj->VAO;
					auto &transform = obj->Transform;

					MultiDrawIndirectCommand drawCmd{};
					drawCmd.BaseInstance = ObjectData.size() + startBaseInstance;
					drawCmd.BaseVertex = s.StartVertex;
					drawCmd.FirstIndex = s.StartIndex;
					drawCmd.Count = s.IndexCount;
					drawCmd.InstanceCount = 1;

					DrawCommands.emplace_back(drawCmd);

					ObjectData.emplace_back(FPerObjectData{transform});

					auto &matRanges = DrawRanges[mat];
					auto &range = matRanges[vao];

					if (range.Count == 0)
					{
						range.First = DrawCommands.size() - 1;
					}

					range.Count++;
				}
			}
		}

		void Draw(FPass &pass, Ref<GeneralBuffer> indirect, uint32_t baseOffset)
		{
			const static uint64_t stride = sizeof(MultiDrawIndirectCommand);

			// render meshes
			for (auto &[mat, vaoMap] : DrawRanges)
			{
				pass.Emplace<CmdBindMaterial>()(mat.get());

				for (auto &[vao, range] : vaoMap)
				{
					uint32_t offset = baseOffset + range.First;
					uint32_t count = range.Count;

					vao->DeclareAccess(pass, EBufferAccess::IndirectRead, EBufferAccess::IndirectRead);

					pass.Emplace<CmdMultiDrawIndexedIndirect>()(ETopologyMode::Triangles, indirect.get(), vao.get(), count, stride, offset);
				}
			}
		}
	};

	void SceneRenderer::Init(const glm::uvec2 &size)
	{
		mSize = size;

		mSceneRenderData = CreateRef<FSceneRenderData>();
		mSceneRenderData->Init();

		// Initialize the framebuffer or any other resources needed for rendering
		FramebufferSpecification specs;
		specs.Size = size;
		specs.Attachments.AddColorAttachment({FTextureCreateInfo{.Format = EFormat::RGBA32F, .WrapMode = EWrapMode::CLAMP_TO_EDGE}});
		specs.Attachments.SetDepthAttachment({FTextureCreateInfo{.Format = EFormat::DEPTH24_STENCIL8, .WrapMode = EWrapMode::CLAMP_TO_EDGE}});
		specs.DebugName = "SceneRenderer";

		mFramebuffer = Framebuffer::Create(specs);

		mCameraUBO = GeneralBuffer::Create(sizeof(FView), EBufferType::UniformBuffer);
		mModelSSBO = GeneralBuffer::Create(sizeof(FPerObjectData) * 10'000, EBufferType::StorageBuffer);
		mIndirectDrawBuffer = GeneralBuffer::Create(sizeof(MultiDrawIndirectCommand) * 10'000, EBufferType::IndirectBuffer);

		mPostProcessStack.Init(size);
		mLights.Init();
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

		mSceneRenderData->ShadowRenderer.Begin();
		mSceneRenderData->Reset();
		renderer.BeginBatching();
		mLights.BeginRecording();
	}

	void SceneRenderer::End()
	{
		mLights.Flush();

		static auto sortOpaque = [=](const Ref<FMeshRenderData> &lhs, const Ref<FMeshRenderData> &rhs)
		{
			float za = (mView.View * glm::vec4(lhs->Transform.GetTranslation(), 1.0f)).z;
			float zb = (mView.View * glm::vec4(rhs->Transform.GetTranslation(), 1.0f)).z;
			return za < zb;
		};

		static auto sortTransparent = [=](const Ref<FMeshRenderData> &lhs, const Ref<FMeshRenderData> &rhs)
		{
			float za = (mView.View * glm::vec4(lhs->Transform.GetTranslation(), 1.0f)).z;
			float zb = (mView.View * glm::vec4(rhs->Transform.GetTranslation(), 1.0f)).z;
			return za > zb; // back-to-front
		};

		for (auto &[mat, data] : mSceneRenderData->OpaqueData)
		{
			std::sort(data.begin(), data.end(), sortOpaque);
		}

		for (auto &[mat, data] : mSceneRenderData->TransparentData)
		{
			std::sort(data.begin(), data.end(), sortTransparent);
		}

		std::sort(mSceneRenderData->ShadowPassRenderData.begin(), mSceneRenderData->ShadowPassRenderData.end(), sortOpaque);
		std::sort(mSceneRenderData->RenderPassRenderData.begin(), mSceneRenderData->RenderPassRenderData.end(), sortOpaque);

		RenderBatch transparentBatch{};
		transparentBatch.Build(mSceneRenderData->TransparentData);

		RenderBatch opaqueBatch{};
		opaqueBatch.Build(mSceneRenderData->OpaqueData, transparentBatch.ObjectData.size());

		std::vector<MultiDrawIndirectCommand> drawCommands;
		drawCommands.insert(drawCommands.end(), transparentBatch.DrawCommands.begin(), transparentBatch.DrawCommands.end());
		drawCommands.insert(drawCommands.end(), opaqueBatch.DrawCommands.begin(), opaqueBatch.DrawCommands.end());

		std::vector<FPerObjectData> objectData;
		objectData.insert(objectData.end(), transparentBatch.ObjectData.begin(), transparentBatch.ObjectData.end());
		objectData.insert(objectData.end(), opaqueBatch.ObjectData.begin(), opaqueBatch.ObjectData.end());

		auto &renderer = Renderer::Get();
		FPassState state{};
		state.Color = {EAttachmentLoadState::Clear, EAttachmentStoreState::Store, {0.1f, 0.1f, 0.1f, 1.0f}};
		state.Depth = {EAttachmentLoadState::Clear, EAttachmentStoreState::Store};
		auto &scenePass = renderer.BeginPass("Scene Renderer", EPassType::OffScreen, state);

		auto environmentMaps = mEnvironment.GetCurrentMaps();
		auto prefilter = environmentMaps.PreFilter;
		auto irradiance = environmentMaps.Irradiance;
		auto brdfLUT = mEnvironment.GetBRDFLUT();

		// global buffers
		scenePass.PushGlobal(0, 0, mCameraUBO);
		scenePass.PushGlobal(0, 1, mLights.GetBuffer());
		scenePass.PushGlobal(3, 0, mModelSSBO);
		scenePass.PushGlobal(0, 2, brdfLUT);
		scenePass.PushGlobal(0, 3, prefilter);
		scenePass.PushGlobal(0, 4, irradiance);

		// opaque pass
		scenePass.BeginPhase(EPhaseType::Graphics);
		mCameraUBO->SetData(&mView, sizeof(FView));
		mModelSSBO->SetData(objectData.data(), objectData.size() * sizeof(FPerObjectData));
		mIndirectDrawBuffer->SetData(drawCommands.data(), drawCommands.size() * sizeof(MultiDrawIndirectCommand));

		scenePass.Push(mFramebuffer);
		scenePass.Push(prefilter, EImageAccess::ColorRead);
		scenePass.Push(irradiance, EImageAccess::ColorRead);
		scenePass.Push(brdfLUT, EImageAccess::ColorRead);
		scenePass.Push(mCameraUBO, EBufferAccess::UniformRead);
		scenePass.Push(mModelSSBO, EBufferAccess::StorageRead);
		scenePass.Push(mLights.GetBuffer(), EBufferAccess::StorageRead);

		uint32_t transparentBase = 0;
		uint32_t opaqueBase = transparentBatch.DrawCommands.size();

		scenePass.Emplace<CmdBindPipeline>()(PipelineRegistry::Get("MESH_OPAQUE"));
		opaqueBatch.Draw(scenePass, mIndirectDrawBuffer, opaqueBase);

		scenePass.Emplace<CmdBindPipeline>()(PipelineRegistry::Get("MESH_TRANSPARENT"));
		transparentBatch.Draw(scenePass, mIndirectDrawBuffer, transparentBase);

		renderer.EndBatching();

		scenePass.EndPhase();

		scenePass.BeginPhase("Transition to read", EPhaseType::Transfer);
		scenePass.Push(mFramebuffer->GetColorAttachment(), EImageAccess::ColorRead);
		scenePass.EndPhase();

		renderer.EndPass();

		// post process
		FPostProcessTextureSet set{mFramebuffer->GetColorAttachment(), mFramebuffer->GetDepthAttachment()};
		mOutputTexture = mPostProcessStack.Build(renderer.GetActiveGraph(), set);
	}

	void SceneRenderer::Submit(const DirectionalLight &light)
	{
		mLights.Submit(light);

		FShadowCascadedCreateInfo shadow_info{};
		shadow_info.LightDirection = light.GetDirection();
		shadow_info.CameraProj = mView.Projection;
		shadow_info.InverseCameraView = mView.View;
		shadow_info.CameraNearFar = mView.NearFar;
		shadow_info.LightCascadeFrustumNear = 1.0f;

		mSceneRenderData->ShadowRenderer.SubmitDirectionalLight(shadow_info);
	}

	void SceneRenderer::Submit(const PointLight &light)
	{
		mLights.Submit(light);

		FShadowCubeCreateInfo shadow_info{};
		shadow_info.LightPosition = light.GetPosition();
		shadow_info.LightNearFar = {1.0f, light.GetRadius()};

		mSceneRenderData->ShadowRenderer.SubmitPointLight(shadow_info);
	}

	void SceneRenderer::Submit(const SpotLight &light)
	{
		/*auto inner = glm::cos(glm::radians(info.InnerCutoff));
		auto outer = glm::cos(glm::radians(info.OuterCutoff));*/
		mLights.Submit(light);

		FShadowFrustumCreateInfo shadow_info{};
		shadow_info.LightDirection = light.GetDirection();
		shadow_info.LightPosition = light.GetPosition();

		// TODO : maybe radius
		shadow_info.LightAngleNearFar = {glm::radians(light.GetOuterAngleDegrees()), .1f, light.GetRadius()};

		mSceneRenderData->ShadowRenderer.SubmitSpotLight(shadow_info);
	}

	void SceneRenderer::Submit(const FMeshInfo &info)
	{
		const auto &mesh = info.Mesh;
		const auto &transform = info.Transform;
		const auto &materials = info.Materials;

		// Cull the mesh if it is not visible
		if (!mesh || IsMeshCulled(mesh, transform))
			return;

		auto &subMeshes = mesh->GetSubMeshes();
		Ref<FStaticMeshRenderData> data;

		for (auto &s : subMeshes)
		{
			auto material = materials.get_material(s.MaterialIndex);
			if (!material)
				return;

			if (mesh->get_type() == rttr::type::get<SkeletalMesh>())
			{
				auto skeletal_data = CreateRef<FSkeletalMeshRenderData>();
				skeletal_data->SubMesh = s;
				skeletal_data->Bones = info.Bones;
				data = skeletal_data;
			}
			else
			{
				auto static_data = CreateRef<FStaticMeshRenderData>();
				static_data->SubMesh = s;
				data = static_data;
			}

			data->VAO = mesh->GetVertexArray();
			data->Transform = info.Transform;
			data->EntityID = info.EntityID;
			data->Instances = info.Instances;

			if (material->IsTransparent())
				mSceneRenderData->TransparentData[material].emplace_back(data);
			else
				mSceneRenderData->OpaqueData[material].emplace_back(data);

			if (material->ShouldCastShadows())
				mSceneRenderData->ShadowPassRenderData.emplace_back(data);
		}
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

	bool SceneRenderer::IsMeshCulled(const Ref<BaseMesh> &mesh, const glm::mat4 &transform)
	{
		if (!mesh)
			return true;

		const auto &bounds = mesh->GetBoundingBox();
		auto volume = FSphereVolume(bounds.GetCenter(), bounds.GetRadius());
		return !volume.InFrustum(mFrustum, FTransform(transform));
	}
} // namespace BHive