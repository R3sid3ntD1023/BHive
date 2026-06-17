#include "core/math/Transform.h"
#include "gfx/Camera.h"
#include "gfx/Framebuffer.h"
#include "gfx/RenderCommand.h"
#include "gfx/Shader.h"
#include "gfx/ShaderManager.h"
#include "gfx/Texture.h"
#include "importers/TextureImporter.h"
#include "gfx/mesh/primitives/Quad.h"
#include "render_passes/Aces.h"
#include "render_passes/Bloom.h"
#include "gfx/renderers/PMREMGenerator.h"
#include "gfx/renderers/Renderer.h"
#include "SceneRenderer.h"
#include "ShadowRenderer.h"

#include "Lights.h"
#include "core/math/boundingbox/AABB.h"
#include "core/math/volumes/SphereVolume.h"
#include "gfx/mesh/SkeletalMesh.h"
#include "gfx/mesh/StaticMesh.h"

#include "gfx/renderers/render_passes/RenderPass.h"

namespace BHive
{
	struct Distance
	{
		static bool Sort(const Ref<FMeshRenderData> &lhs, const Ref<FMeshRenderData> &rhs)
		{
			auto& view = Renderer::Get().GetViewSystem().GetMainView();
			glm::vec3 viewPosA =  view.View * glm::vec4(lhs->Transform.GetTranslation(), 1.0f);
			glm::vec3 viewPosB =  view.View * glm::vec4(rhs->Transform.GetTranslation(), 1.0f);
			return viewPosA.z < viewPosB.z;
		}
	};

	struct FSceneRenderData
	{
		std::unordered_map<Ref<Material>, FMeshRenderDatas> RenderData;
		FMeshRenderDatas ShadowPassRenderData;
		FMeshRenderDatas RenderPassRenderData;

		ShadowRenderer ShadowRenderer;

		void Init()
		{
			ShadowRenderer.Init(MAX_LIGHTS);
		}

		void Reset()
		{
			RenderData.clear();
			ShadowPassRenderData.clear();
			RenderPassRenderData.clear();
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
		specs.Attachments.attach({.Format = EFormat::RGBA32F, .WrapMode = EWrapMode::CLAMP_TO_EDGE})
			.attach({.Format = EFormat::DEPTH24_STENCIL8, .WrapMode = EWrapMode::CLAMP_TO_EDGE});

		mFramebuffer = Framebuffer::Create(specs);

		// Create a final framebuffer for post-processing effects
		specs.Attachments.reset();
		specs.Attachments.attach({.Format = EFormat::RGBA8, .WrapMode = EWrapMode::CLAMP_TO_EDGE});
		specs.Attachments.attach({EFormat::DEPTH24_STENCIL8});
		mFinalFramebuffer = Framebuffer::Create(specs);

		// Create a quad for rendering the final output
		mQuad = CreateRef<PQuad>();
		mQuadShader = ShaderManager::Get(ENGINE_SHADER_PATH "/ScreenQuad.glsl");

		// add default post-processing effects
		PushPostProcessRenderPass(CreateRef<BloomRenderPass>());
		PushPostProcessRenderPass(CreateRef<AcesRenderPass>());
	}

	void SceneRenderer::Begin(const Camera *camera, const FTransform &view)
	{
		Renderer::Get().BeginFrame();
		Renderer::Get().SubmitCamera(camera->GetProjection(), view.Inverse());

		mSceneRenderData->ShadowRenderer.Begin();
		mSceneRenderData->Reset();
	}

	void SceneRenderer::End()
	{
		for (auto &[mat, data] : mSceneRenderData->RenderData)
		{
			std::sort(data.begin(), data.end(), Distance::Sort);
		}

		std::sort(mSceneRenderData->ShadowPassRenderData.begin(), mSceneRenderData->ShadowPassRenderData.end(), Distance::Sort);
		std::sort(mSceneRenderData->RenderPassRenderData.begin(), mSceneRenderData->RenderPassRenderData.end(), Distance::Sort);

		while (mCommands.size())
		{
			mCommands.top()();
			mCommands.pop();
		}

		mSceneRenderData->ShadowRenderer.End();
		mSceneRenderData->ShadowRenderer.Render(mSceneRenderData->ShadowPassRenderData);

		for (auto &render_pass : mRenderPasses)
		{
			if (!render_pass->IsEnabled())
			{
				continue;
			}

			render_pass->Render(mSceneRenderData->RenderPassRenderData);
		}

		mFramebuffer->Bind();

		Renderer::Get().ClearColor(0.1f, 0.1f, 0.1f, 0.0f);

		Renderer::Get().Clear();

		// render meshes
		for (auto &[mat, objects] : mSceneRenderData->RenderData)
		{
			/*EnvironmentMapGenerator.GetPreFilteredEnvironmentTetxure()->Bind(6);
			EnvironmentMapGenerator.GetIrradianceTexture()->Bind(7);
			EnvironmentMapGenerator.GetBDRFLUT()->Bind(8);*/

			static uint32_t shadow_map_bindings[] = {9, 10, 11};
			mSceneRenderData->ShadowRenderer.BindShadowMaps(shadow_map_bindings);

			mat->Submit();

			/*for (const auto &object : objects)
				Renderer::Draw(object);*/
		}

		Renderer::Get().EndFrame();

		mFramebuffer->UnBind();

		auto texture = mFramebuffer->GetColorAttachment(0);

		for (auto &effect : mPostProcessRenderPasses)
		{
			if (!effect->IsEnabled())
				continue;

			effect->Process(texture);
			texture = effect->GetOutputTexture();
		}

		mFinalFramebuffer->Bind();

		Renderer::Get().Clear();

		//mQuadShader->Bind();

		//texture->Bind();

		Renderer::Get().DrawElements(ETopologyMode::Triangles, mQuad->GetVertexArray().get());

		mFinalFramebuffer->UnBind();
	}

	void SceneRenderer::SubmitLight(const FDirectionalLight & light)
	{
		auto &camera = Renderer::Get().GetViewSystem().GetMainView();

		Renderer::Get().Light.Submit(light);

		FShadowCascadedCreateInfo shadow_info{};
		shadow_info.LightDirection = light.Direction;
		shadow_info.CameraProj = camera.Projection;
		shadow_info.InverseCameraView = camera.View;
		shadow_info.CameraNearFar = camera.NearFar;
		shadow_info.LightCascadeFrustumNear = 1.0f;

		mSceneRenderData->ShadowRenderer.SubmitDirectionalLight(shadow_info);
	}

	void SceneRenderer::SubmitLight(const FPointLight & light)
	{
		Renderer::Get().Light.Submit(light);

		FShadowCubeCreateInfo shadow_info{};
		shadow_info.LightPosition = light.Position;
		shadow_info.LightNearFar = {1.0f, light.Position.w};

		mSceneRenderData->ShadowRenderer.SubmitPointLight(shadow_info);
	}

	void SceneRenderer::SubmitLight(const FSpotLight & light)
	{
		/*auto inner = glm::cos(glm::radians(info.InnerCutoff));
		auto outer = glm::cos(glm::radians(info.OuterCutoff));*/
		Renderer::Get().Light.Submit(light);

		FShadowFrustumCreateInfo shadow_info{};
		shadow_info.LightDirection = light.Direction;
		shadow_info.LightPosition = light.Position;
		shadow_info.LightAngleNearFar = {light.Params.x, .1f, light.Position.w};

		mSceneRenderData->ShadowRenderer.SubmitSpotLight(shadow_info);
	}

	void SceneRenderer::SubmitMesh(const FMeshInfo &info)
	{
		const auto &mesh = info.Mesh;
		const auto &transform = info.Transform;
		const auto &materials = info.Materials;

		Ref<FStaticMeshRenderData> data;

		// Cull the mesh if it is not visible
		if (!mesh || IsMeshCulled(mesh, transform))
			return;

		auto &sub_meshes = mesh->GetSubMeshes();

		for (auto &sub_mesh : sub_meshes)
		{
			auto material = materials.get_material(sub_mesh.MaterialIndex);
			if (!material)
				return;

			if (mesh->get_type() == rttr::type::get<SkeletalMesh>())
			{
				auto skeletal_data = CreateRef<FSkeletalMeshRenderData>();
				skeletal_data->SubMesh = sub_mesh;
				skeletal_data->Bones = info.Bones;
				data = skeletal_data;
			}
			else
			{
				auto static_data = CreateRef<FStaticMeshRenderData>();
				static_data->SubMesh = sub_mesh;
				data = static_data;
			}

			data->VertexArray = mesh->GetVertexArray();
			data->Transform = info.Transform;
			data->EntityID = info.EntityID;
			data->Instances = info.Instances;

			mSceneRenderData->RenderPassRenderData.emplace_back(data);
			mSceneRenderData->RenderData[material].emplace_back(data);

			if (material->ShouldCastShadows())
				mSceneRenderData->ShadowPassRenderData.emplace_back(data);
		}
	}

	void SceneRenderer::SubmitCommand(const Command &cmd)
	{
		mCommands.push(cmd);
	}

	float SceneRenderer::GetDistanceToCamera(const FTransform &transform)
	{
		const auto &C = Renderer::Get().GetViewSystem().GetMainView().Position;
		return glm::distance(glm::vec3(C), transform[2]);
	}

	void SceneRenderer::Resize(const glm::uvec2 &size)
	{
		mSize = size;

		mFramebuffer->Resize(size);

		mFinalFramebuffer->Resize(size);

		for (auto &render_pass : mRenderPasses)
		{
			render_pass->Resize(size);
		}

		for (auto &post_process : mPostProcessRenderPasses)
		{
			post_process->Resize(size);
		}

		Renderer::Get().SetViewport(0, 0, size.x, size.y);
	}

	Ref<Texture> SceneRenderer::GetColorAttachment(uint32_t index) const
	{
		return mFinalFramebuffer->GetColorAttachment(index);
	}

	Ref<Texture> SceneRenderer::GetDepthAttachment() const
	{
		return mFinalFramebuffer->GetDepthAttachment();
	}

	void SceneRenderer::RenderToScreen()
	{
		mFinalFramebuffer->BlitToWindow(0, 0, mSize.x, mSize.y);
	}

	bool SceneRenderer::IsMeshCulled(const Ref<BaseMesh> &mesh, const glm::mat4 &transform)
	{
		if (!mesh)
			return true;

		const auto &bounds = mesh->GetBoundingBox();
		const auto &frustum = Renderer::Get().GetFrustum();

		auto volume = FSphereVolume(bounds.GetCenter(), bounds.GetRadius());
		return !volume.InFrustum(frustum, FTransform(transform));
	}

	void SceneRenderer::PushPostProcessRenderPass(const Ref<PostProcessRenderPass> &pass)
	{
		mPostProcessRenderPasses.push_back(pass);
		pass->Init();
		pass->CreateResizableObjects(mSize);
	}

	void SceneRenderer::PushRenderPass(const Ref<RenderPass> &render_pass)
	{
		mRenderPasses.push_back(render_pass);
		render_pass->Init();
		render_pass->CreateResizableObjects(mSize);
	}

	SceneRenderer::PostProcessPasses &SceneRenderer::GetPostProcessPasses()
	{
		return mPostProcessRenderPasses;
	}
} // namespace BHive