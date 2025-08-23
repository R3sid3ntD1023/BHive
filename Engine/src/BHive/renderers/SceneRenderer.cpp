#include "SceneRenderer.h"
#include "renderers/Renderer.h"
#include "gfx/Camera.h"
#include "gfx/Framebuffer.h"
#include "gfx/Shader.h"
#include "gfx/ShaderManager.h"
#include "mesh/primitives/Quad.h"
#include "gfx/RenderCommand.h"
#include "core/math/Transform.h"
#include "renderers/postprocessing/Bloom.h"
#include "renderers/PMREMGenerator.h"
#include "importers/TextureImporter.h"
#include "postprocessing/Aces.h"
#include "gfx/textures/Texture2D.h"
#include "ShadowRenderer.h"

#include "mesh/StaticMesh.h"
#include "mesh/SkeletalMesh.h"
#include "core/math/boundingbox/AABB.h"
#include "core/math/volumes/SphereVolume.h"
#include "gfx/StorageBuffer.h"
#include "gfx/UniformBuffer.h"
#include "buffers/LightBuffer.h"

namespace BHive
{

	struct FSceneRenderData
	{
		std::unordered_map<Ref<Material>, FMeshRenderDatas> RenderData;
		FMeshRenderDatas ShadowPassRenderData;

		LightBuffer Lights;
		ShadowRenderer ShadowRenderer;

		void Init()
		{
			Lights.Init();
			ShadowRenderer.Init(MAX_LIGHTS);
		}

		void Reset()
		{
			RenderData.clear();
			ShadowPassRenderData.clear();
		}
	};

	void SceneRenderer::Initialize(uint32_t width, uint32_t height)
	{
		mSceneRenderData = CreateRef<FSceneRenderData>();
		mSceneRenderData->Init();

		// Initialize the framebuffer or any other resources needed for rendering
		FramebufferSpecification specs;
		specs.Width = width;
		specs.Height = height;
		specs.Attachments.attach({.InternalFormat = EFormat::RGBA8, .WrapMode = EWrapMode::CLAMP_TO_EDGE}).attach({.InternalFormat = EFormat::DEPTH24_STENCIL8, .WrapMode = EWrapMode::CLAMP_TO_EDGE});

		mFramebuffer = CreateRef<Framebuffer>(specs);

		// Initialize bloom post-processing effect if enabled
		AddPostProcessingEffect(CreateRef<Bloom>(5, width, height, FBloomSettings{}));
		AddPostProcessingEffect(CreateRef<Aces>(width, height));

		// Create a final framebuffer for post-processing effects
		specs.Attachments.reset();
		specs.Attachments.attach({.InternalFormat = EFormat::RGBA8, .WrapMode = EWrapMode::CLAMP_TO_EDGE}).attach({.InternalFormat = EFormat::DEPTH24_STENCIL8, .WrapMode = EWrapMode::CLAMP_TO_EDGE});
		mFinalFramebuffer = CreateRef<Framebuffer>(specs);

		// Create a quad for rendering the final output
		mQuad = CreateRef<PQuad>();
		mQuadShader = ShaderManager::Get().Load(ENGINE_SHADER_PATH "/ScreenQuad.glsl");

		// Initialize the PMREM generator
		EnvironmentMapGenerator.Initialize();

		// Load the environment map if it is not already set
		if (!sEnvironmentMap)
		{
			sEnvironmentMap = TextureLoader::Import(ENGINE_PATH "/data/hdr/industrial_sunset_puresky_2k.hdr");
			EnvironmentMapGenerator.SetEnvironmentMap(sEnvironmentMap);
		}

		mRenderSize = {width, height};
	}

	void SceneRenderer::Begin(const Camera *camera, const FTransform &view)
	{
		Renderer::Begin();
		Renderer::SubmitCamera(camera->GetProjection(), view.Inverse());

		mSceneRenderData->ShadowRenderer.Begin();
		mSceneRenderData->Lights.Begin();
		mSceneRenderData->Reset();
	}

	void SceneRenderer::End()
	{
		mSceneRenderData->Lights.End();
		mSceneRenderData->ShadowRenderer.End();

		mSceneRenderData->ShadowRenderer.Render(mSceneRenderData->ShadowPassRenderData);

		mFramebuffer->Bind();

		RenderCommand::Clear();

		EnvironmentMapGenerator.GetPreFilteredEnvironmentTetxure()->Bind(6);
		EnvironmentMapGenerator.GetIrradianceTexture()->Bind(7);
		EnvironmentMapGenerator.GetBDRFLUT()->Bind(8);

		static uint32_t shadow_map_bindings[] = {9, 10, 11};
		mSceneRenderData->ShadowRenderer.BindShadowMaps(shadow_map_bindings);

		// render meshes
		for (auto &[mat, objects] : mSceneRenderData->RenderData)
		{
			auto shader = mat->GetShader();
			shader->Bind();
			mat->Submit(shader);

			for (auto [dist, object] : objects)
				Renderer::SubmitMesh(object);

			shader->UnBind();
		}

		while (mCommands.size())
		{
			mCommands.top()();
			mCommands.pop();
		}

		Renderer::End();

		mFramebuffer->UnBind();

		auto texture = mFramebuffer->GetColorAttachment(0);

		for (const auto &effect : mPostProcessingEffects)
		{
			texture = effect->Process(texture);
		}

		mFinalFramebuffer->Bind();

		RenderCommand::Clear();

		mQuadShader->Bind();

		texture->Bind();

		RenderCommand::DrawElements(EDrawMode::Triangles, *mQuad->GetVertexArray());

		mFinalFramebuffer->UnBind();
	}

	void SceneRenderer::SetEnvironmentMap(const Ref<Texture2D> &environment)
	{
		sEnvironmentMap = environment;
		EnvironmentMapGenerator.SetEnvironmentMap(environment);
	}

	void SceneRenderer::AddPostProcessingEffect(const Ref<PostProcessor> &processor)
	{
		mPostProcessingEffects.push_back(processor);
	}

	void SceneRenderer::SubmitLight(const DirectionalLight &light, const glm::vec3 &direction)
	{
		auto &camera = Renderer::GetCamera().GetCameraData();
		mSceneRenderData->Lights.Submit(FDirectionalLightInfo{light.Color, direction});

		FShadowCascadedCreateInfo info{};
		info.LightDirection = direction;
		info.CameraProj = camera.Projection;
		info.InverseCameraView = camera.View;
		info.CameraNearFar = camera.NearFar;
		info.LightCascadeFrustumNear = 1.0f;

		mSceneRenderData->ShadowRenderer.SubmitDirectionalLight(info);
	}

	void SceneRenderer::SubmitLight(const PointLight &light, const glm::vec3 &position)
	{
		mSceneRenderData->Lights.Submit(FPointLightInfo{light.Color, position, light.Radius});

		FShadowCubeCreateInfo info{};
		info.LightPosition = position;
		info.LightNearFar = {1.0f, light.Radius};

		mSceneRenderData->ShadowRenderer.SubmitPointLight(info);
	}

	void SceneRenderer::SubmitLight(const SpotLight &light, const glm::vec3 &direction, const glm::vec3 &position)
	{
		auto inner = glm::cos(glm::radians(light.InnerCutOff));
		auto outer = glm::cos(glm::radians(light.OuterCutOff));
		mSceneRenderData->Lights.Submit(FSpotLightInfo{light.Color, position, direction, light.Radius, inner, outer});

		FShadowFrustumCreateInfo info;
		info.LightDirection = direction;
		info.LightAngleNearFar = {light.OuterCutOff, .1f, light.Radius};
		info.LightPosition = position;

		mSceneRenderData->ShadowRenderer.SubmitSpotLight(info);
	}

	void SceneRenderer::SubmitMesh(const Ref<StaticMesh> &mesh, const glm::mat4 &transform, const glm::mat4 *instances, size_t instanceCount)
	{
		SubmitMesh(mesh, mesh->GetMaterialTable(), transform, instances, instanceCount);
	}

	void SceneRenderer::SubmitMesh(const Ref<SkeletalMesh> &mesh, const SkeletalPose &pose, const glm::mat4 &transform, const glm::mat4 *instances, size_t instanceCount)
	{
		SubmitMesh(mesh, mesh->GetMaterialTable(), pose, transform, instances, instanceCount);
	}

	void SceneRenderer::SubmitMesh(const Ref<StaticMesh> &mesh, const MaterialTable &materials, const glm::mat4 &transform, const glm::mat4 *instances, size_t instanceCount)
	{
		if (!mesh || IsMeshCulled(mesh, transform))
			return;

		auto &sub_meshes = mesh->GetSubMeshes();

		for (auto &sub_mesh : sub_meshes)
		{
			auto material = materials.get_material(sub_mesh.MaterialIndex);
			if (!material)
				return;

			auto data = CreateRef<FMeshRenderData>();
			data->VertexArray = mesh->GetVertexArray();
			data->SubMesh = sub_mesh;
			data->Transform = transform;
			data->Instances = instances;
			data->InstanceCount = instanceCount;

			float distance = GetDistanceToCamera(transform);
			mSceneRenderData->RenderData[material].emplace(distance, data);

			if (material->ShouldCastShadows())
				mSceneRenderData->ShadowPassRenderData.emplace(distance, data);
		}
	}

	void
	SceneRenderer::SubmitMesh(const Ref<SkeletalMesh> &mesh, const MaterialTable &materials, const SkeletalPose &pose, const glm::mat4 &transform, const glm::mat4 *instances, size_t instanceCount)
	{
		if (!mesh || IsMeshCulled(mesh, transform))
			return;

		auto &sub_meshes = mesh->GetSubMeshes();
		for (auto &sub_mesh : sub_meshes)
		{
			auto material = materials.get_material(sub_mesh.MaterialIndex);
			if (!material)
				return;

			auto bones = pose.GetTransformsJointSpace();
			auto data = CreateRef<FSkeletalMeshRenderData>();
			data->VertexArray = mesh->GetVertexArray();
			data->SubMesh = sub_mesh;
			data->Transform = transform;
			data->Instances = instances;
			data->InstanceCount = instanceCount;
			data->Bones = bones;

			float distance = GetDistanceToCamera(transform);
			mSceneRenderData->RenderData[material].emplace(distance, data);

			if (material->ShouldCastShadows())
				mSceneRenderData->ShadowPassRenderData.emplace(distance, data);
		}
	}

	void SceneRenderer::SubmitCommand(const std::function<void()> cmd)
	{
		mCommands.push(cmd);
	}

	float SceneRenderer::GetDistanceToCamera(const FTransform &transform)
	{
		const auto &C = Renderer::GetCamera().GetCameraData().Position;
		return glm::distance(glm::vec3(C), transform[2]);
	}

	void SceneRenderer::Resize(uint32_t width, uint32_t height)
	{
		mFramebuffer->Resize(width, height);

		mFinalFramebuffer->Resize(width, height);

		for (auto &effect : mPostProcessingEffects)
		{
			effect->Resize(width, height);
		}

		mRenderSize = {width, height};
	}

	const Ref<Texture> &SceneRenderer::GetColorAttachment(uint32_t index) const
	{
		return mFinalFramebuffer->GetColorAttachment(index);
	}

	const Ref<Texture> &SceneRenderer::GetDepthAttachment() const
	{
		return mFinalFramebuffer->GetDepthAttachment();
	}

	const Ref<Texture2D> &SceneRenderer::GetEnvironmentMap() const
	{
		return sEnvironmentMap;
	}

	glm::uvec2 SceneRenderer::GetSize() const
	{
		return mRenderSize;
	}

	void SceneRenderer::RenderToScreen()
	{
		RenderCommand::Clear();

		mQuadShader->Bind();

		mFinalFramebuffer->GetColorAttachment()->Bind();

		RenderCommand::DrawElements(EDrawMode::Triangles, *mQuad->GetVertexArray());
	}

	bool SceneRenderer::IsMeshCulled(const Ref<BaseMesh> &mesh, const glm::mat4 &transform)
	{
		if (!mesh)
			return true;

		const auto &bounds = mesh->GetBoundingBox();
		const auto &frustum = Renderer::GetCamera().GetViewFrustum();

		auto volume = FSphereVolume(bounds.GetCenter(), bounds.GetRadius());
		return !volume.InFrustum(frustum, FTransform(transform));
	}

	REFLECT(FRenderSettings)
	{
		BEGIN_REFLECT(FRenderSettings);
	}

	REFLECT(SceneRenderer)
	{
		BEGIN_REFLECT(SceneRenderer)
		REFLECT_PROPERTY("PostProcessEffects", mPostProcessingEffects)
		REFLECT_PROPERTY("EnvironmentMap", GetEnvironmentMap, SetEnvironmentMap);
	}
} // namespace BHive