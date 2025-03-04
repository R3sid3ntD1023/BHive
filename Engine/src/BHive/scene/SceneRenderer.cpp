#include "gfx/RenderCommand.h"
#include "gfx/Shader.h"
#include "gfx/Texture.h"
#include "gfx/UniformBuffer.h"
#include "gfx/VertexArray.h"
#include "renderers/ShadowRenderer.h"
#include "SceneRenderer.h"
#include <glad/glad.h>

#include "core/debug/Instrumentor.h"
#include "mesh/SkeletalPose.h"

#include "importers/TextureImporter.h"
#include "renderers/HDRConverter.h"

#include "mesh/primitives/Sphere.h"
#include "mesh/SkeletalMesh.h"
#include "mesh/StaticMesh.h"

#include "renderers/postprocessing/Bloom.h"
#include "core/profiler/CPUGPUProfiler.h"
#include "gfx/ShaderManager.h"

#include "mesh/primitives/Plane.h"

#define DRAW_ELEMENTS()                    \
	RenderCommand::DrawElementsBaseVertex( \
		EDrawMode::Triangles, *vao, sub_mesh->StartVertex, sub_mesh->StartIndex, sub_mesh->IndexCount);

// #define RENDER_SHADOWS

namespace BHive
{
	SceneRenderer::SceneRenderer(const glm::ivec2 &size, uint32_t flags)
		: mViewportSize(size),
		  mFlags(flags)
	{
		FramebufferSpecification fbspec{};

		fbspec.Attachments.attach({.InternalFormat = EFormat::RGBA32F, .WrapMode = EWrapMode::CLAMP_TO_EDGE})
			.attach({.InternalFormat = EFormat::DEPTH24_STENCIL8, .WrapMode = EWrapMode::CLAMP_TO_EDGE});

		fbspec.Width = size.x;
		fbspec.Height = size.y;
		fbspec.Samples = 16;
		mMultiSampleFramebuffer = CreateRef<Framebuffer>(fbspec);

		fbspec.Samples = 1;
		mFramebuffer = CreateRef<Framebuffer>(fbspec);

		fbspec.Attachments.reset().attach({.InternalFormat = EFormat::RGBA32F, .WrapMode = EWrapMode::CLAMP_TO_EDGE});

		fbspec.Samples = 1;
		mQuadFramebuffer = CreateRef<Framebuffer>(fbspec);

		mScreenQuad = CreateRef<PPlane>(1.f, 1.f);

		mCube = CreateRef<PSphere>(300.f);

		mQuadShader = ShaderManager::Get().Load(ENGINE_PATH "/data/shaders/ScreenQuad.glsl");

		mSkyBoxShader = ShaderManager::Get().Load(ENGINE_PATH "/data/shaders/SkyBox.glsl");

		auto environment_texture = TextureImporter::Import(ENGINE_PATH "/data/hdr/industrial_sunset_puresky_2k.hdr");
		HDRConverter::Get().SetEnvironmentMap(environment_texture);

		mSceneData.mOpaqueObjects.reserve(100);
		mSceneData.mTransparentObjects.reserve(100);
		mSceneData.mShadowCasters.reserve(100);

		if ((flags & ESceneRendererFlags_NoShadows) == 0)
		{
			ShadowRenderer::GetShadowFBO()->GetDepthAttachment()->Bind(3);
			ShadowRenderer::GetSpotShadowFBO()->GetDepthAttachment()->Bind(4);
			ShadowRenderer::GetPointShadowFBO()->GetDepthAttachment()->Bind(5);
		}

		FBloomSettings settings{};
		mPostProcessors.push_back(CreateRef<Bloom>(5, size.x, size.y, settings));
	}

	SceneRenderer::~SceneRenderer()
	{
	}

	void SceneRenderer::Begin(const Camera &camera, const FTransform &view)
	{
		mSceneData.mProjection = camera.GetProjection();
		mSceneData.mView = view;
		mSceneData.clear();

		Renderer::Begin(mSceneData.mProjection, mSceneData.mView);
	}

	void SceneRenderer::End()
	{
		CPU_PROFILER_FUNCTION();

		if ((mFlags & ESceneRendererFlags_NoShadows) == 0)
		{
			RenderShadows(mSceneData.mLights.size() && mSceneData.mShadowCasters.size());
		}

		mMultiSampleFramebuffer->Bind();

		RenderCommand::ClearColor(.1f, .1f, .1f, 1.0f);

		RenderCommand::Clear();
		RenderCommand::EnableBlend(false);

		// RENDER Skybox

		RenderCommand::CullFront();

		mSkyBoxShader->Bind();
		HDRConverter::Get().GetEnvironmentCaptureFBO()->GetColorAttachment()->Bind();

		RenderCommand::DepthFunc(GL_LEQUAL);

		auto &submesh = mCube->GetSubMeshes()[0];
		RenderCommand::DrawElementsBaseVertex(
			EDrawMode::Triangles, *mCube->GetVertexArray(), submesh.StartVertex, submesh.StartIndex, submesh.IndexCount);

		mSkyBoxShader->UnBind();

		RenderCommand::CullBack();

		RenderCommand::DepthFunc(GL_LESS);

		// RENDER OPAQUE OBJECTS SORTED

		auto &opaque = mSceneData.mOpaqueObjects;
		std::sort(
			opaque.begin(), opaque.end(),
			[this](const FObjectMaterialData &a, const FObjectMaterialData &b)
			{
				float dist_a = glm::distance(glm::vec3(mSceneData.mView[3]), a.mObjectData.mTransform.get_translation());
				float dist_b = glm::distance(glm::vec3(mSceneData.mView[3]), b.mObjectData.mTransform.get_translation());

				return dist_a > dist_b;
			});

		GPU_PROFILER_SCOPED("Opaque Objects");

		for (auto &[material, objectdata] : opaque)
		{
			auto &[transform, sub_mesh, vao, joints] = objectdata;

			auto shader = material->Submit();
			shader->SetUniform("u_global_flags", mFlags);
			if (joints.size())
			{
				Renderer::SubmitSkeletalMesh(joints);
			}

			DRAW_ELEMENTS();
		}

		RenderCommand::EnableBlend(true);

		// RENDER TRANSPARENT OBJECTS SORTED
		GPU_PROFILER_SCOPED("Transparent Objects");
		auto &transparent = mSceneData.mTransparentObjects;
		std::sort(
			transparent.begin(), transparent.end(),
			[this](const FObjectMaterialData &a, const FObjectMaterialData &b)
			{
				float dist_a = glm::distance(glm::vec3(mSceneData.mView[3]), a.mObjectData.mTransform.get_translation());
				float dist_b = glm::distance(glm::vec3(mSceneData.mView[3]), b.mObjectData.mTransform.get_translation());

				return dist_a < dist_b;
			});

		for (auto &[material, objectdata] : transparent)
		{
			auto &[transform, sub_mesh, vao, joints] = objectdata;

			auto shader = material->Submit();
			shader->SetUniform("u_global_flags", mFlags);

			if (joints.size())
			{
				Renderer::SubmitSkeletalMesh(joints);
			}

			DRAW_ELEMENTS();
		}

		RenderCommand::EnableBlend(false);

		for (auto &func : mRenderQueue)
		{
			func();
		}

		Renderer::End();

		mMultiSampleFramebuffer->UnBind();

		mMultiSampleFramebuffer->Blit(mFramebuffer);

		// post process
		Ref<Texture> post_process_texture = nullptr;
		for (auto &postprocess : mPostProcessors)
		{
			post_process_texture = postprocess->Process(mFramebuffer->GetColorAttachment());
		}

		// render screen quad

		mQuadFramebuffer->Bind();

		RenderCommand::DisableDepth();

		RenderCommand::ClearColor(.2f, .2f, .2f, 1.0f);
		RenderCommand::Clear(Buffer_Color);

		mQuadShader->Bind();
		mQuadShader->SetUniform("u_bloomstrength", mRenderSettings.mStrength);

		mFramebuffer->GetColorAttachment()->Bind();
		post_process_texture->Bind(1);
		RenderCommand::DrawElements(Triangles, *mScreenQuad->GetVertexArray());

		mQuadShader->UnBind();

		mQuadFramebuffer->UnBind();

		RenderCommand::ClearColor(0, 0, 0, 1.0f);
		RenderCommand::Clear(Buffer_Color);

		mQuadShader->Bind();
		mQuadShader->SetUniform("u_postprocess_mode", mRenderSettings.mProcessMode);

		mQuadFramebuffer->GetColorAttachment()->Bind();
		RenderCommand::DrawElements(Triangles, *mScreenQuad->GetVertexArray());

		mQuadShader->UnBind();

		RenderCommand::EnableDepth();

		mRenderQueue.clear();
	}

	void SceneRenderer::SubmitLight(const DirectionalLight &light, const FTransform &transform)
	{
		mSceneData.mLights[ELightType::Directional].push_back({&light, transform});
		Renderer::SubmitDirectionalLight(transform.get_forward(), light);
	}

	void SceneRenderer::SubmitLight(const PointLight &light, const FTransform &transform)
	{
		mSceneData.mLights[ELightType::Point].push_back({&light, transform});
		Renderer::SubmitPointLight(transform.get_translation(), light);
	}

	void SceneRenderer::SubmitLight(const SpotLight &light, const FTransform &transform)
	{
		mSceneData.mLights[ELightType::SpotLight].push_back({&light, transform});
		Renderer::SubmitSpotLight(transform.get_forward(), transform.get_translation(), light);
	}

	void SceneRenderer::SubmitStaticMesh(
		const Ref<StaticMesh> &static_mesh, const FTransform &transform, const Ref<Material> &material)
	{
		if (!static_mesh)
			return;

		auto vao = static_mesh->GetVertexArray();
		for (auto &sub_mesh : *static_mesh)
		{
			if (!material)
				return;

			FObjectData object_data = {transform, &sub_mesh, vao};
			FObjectMaterialData material_data = {material, object_data};

			material->IsTransparent() ? mSceneData.mTransparentObjects.push_back(material_data)
									  : mSceneData.mOpaqueObjects.push_back(material_data);

			if (material->CastShadows())
				mSceneData.mShadowCasters.push_back(object_data);
		}
	}

	void SceneRenderer::SubmitStaticMesh(
		const Ref<StaticMesh> &static_mesh, const FTransform &transform, const MaterialTable &materials)
	{
		if (!static_mesh)
			return;

		auto vao = static_mesh->GetVertexArray();
		for (auto &sub_mesh : *static_mesh)
		{
			auto material = materials.get_material(sub_mesh.MaterialIndex).get();
			if (!material)
				return;

			FObjectData object_data = {transform, &sub_mesh, vao};
			FObjectMaterialData material_data = {material, object_data};

			material->IsTransparent() ? mSceneData.mTransparentObjects.push_back(material_data)
									  : mSceneData.mOpaqueObjects.push_back(material_data);

			if (material->CastShadows())
				mSceneData.mShadowCasters.push_back(object_data);
		}
	}

	void SceneRenderer::SubmitSkeletalMesh(
		const Ref<SkeletalMesh> &skeletal_mesh, const FTransform &transform, const std::vector<glm::mat4> &joints,
		const MaterialTable &materials)
	{
		if (!skeletal_mesh)
			return;

		auto vao = skeletal_mesh->GetVertexArray();
		for (auto &sub_mesh : *skeletal_mesh)
		{
			auto material = materials.get_material(sub_mesh.MaterialIndex).get();

			if (!material)
				return;

			FObjectData object_data = {transform, &sub_mesh, vao, joints};
			FObjectMaterialData material_data = {material, object_data};

			material->IsTransparent() ? mSceneData.mTransparentObjects.push_back(material_data)
									  : mSceneData.mOpaqueObjects.push_back(material_data);

			if (material->CastShadows())
				mSceneData.mShadowCasters.push_back(object_data);
		}
	}

	void SceneRenderer::SubmitRenderQueue(const std::function<void()> &func)
	{
		mRenderQueue.emplace_back(func);
	}

	void SceneRenderer::Resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0)
			return;

		mViewportSize = {width, height};
		mMultiSampleFramebuffer->Resize(width, height);
		mFramebuffer->Resize(width, height);
		mQuadFramebuffer->Resize(width, height);

		for (auto &postprocess : mPostProcessors)
			postprocess->Resize(width, height);
	}

	const Ref<Framebuffer> &SceneRenderer::GetFramebuffer() const
	{
		return mFramebuffer;
	}

	const Ref<Framebuffer> &SceneRenderer::GetFinalFramebuffer() const
	{
		return mQuadFramebuffer;
	}

	void SceneRenderer::RenderShadows(bool render_shadows)
	{
		if (!render_shadows)
			return;

		ShadowRenderer::Begin();

		auto &directional_lights = mSceneData.mLights[ELightType::Directional];
		auto &pointlights = mSceneData.mLights[ELightType::Point];
		auto &spot_lights = mSceneData.mLights[ELightType::SpotLight];

		bool render_directional_shadows = directional_lights.size();
		bool render_point_shadows = pointlights.size();
		bool render_spot_shadows = spot_lights.size();

		if (render_directional_shadows)
		{
			for (auto &light : directional_lights)
			{
				auto &transform = light.mTransform;
				ShadowRenderer::SubmitDirectionalLight(transform.get_forward(), mSceneData.mProjection, mSceneData.mView);
			}

			ShadowRenderer::BeginShadowPass();

			for (auto &object : mSceneData.mShadowCasters)
			{
				auto &[transform, sub_mesh, vao, joints] = object;
				if (joints.size())
				{
					Renderer::SubmitSkeletalMesh(joints);
				}

				DRAW_ELEMENTS();
			}

			ShadowRenderer::EndShadowPass();
		}

		if (render_point_shadows)
		{
			for (auto &light : pointlights)
			{
				auto &transform = light.mTransform;
				ShadowRenderer::SubmitPointLight(transform.get_translation(), Cast<PointLight>(light.mLight)->mRadius);
			}

			ShadowRenderer::BeginPointShadowPass();

			for (auto &object : mSceneData.mShadowCasters)
			{
				auto &[transform, sub_mesh, vao, joints] = object;
				if (joints.size())
				{
					Renderer::SubmitSkeletalMesh(joints);
				}

				DRAW_ELEMENTS();
			}

			ShadowRenderer::EndPointShadowPass();
		}

		if (render_spot_shadows)
		{
			for (auto &light : spot_lights)
			{
				auto &transform = light.mTransform;
				ShadowRenderer::SubmitSpotLight(
					transform.get_forward(), transform.get_translation(), Cast<SpotLight>(light.mLight)->mRadius);
			}

			ShadowRenderer::BeginSpotShadowPass();

			for (auto &object : mSceneData.mShadowCasters)
			{
				auto &[transform, sub_mesh, vao, joints] = object;
				if (joints.size())
				{
					Renderer::SubmitSkeletalMesh(joints);
				}

				DRAW_ELEMENTS();
			}

			ShadowRenderer::EndSpotShadowPass();

			ShadowRenderer::End();
		}
	}

	REFLECT(EPostProcessMode)
	{
		BEGIN_REFLECT_ENUM(EPostProcessMode)
		(ENUM_VALUE(EPostProcessMode_None), ENUM_VALUE(EPostProcessMode_ACES));
	}

	REFLECT(FRenderSettings)
	{
		BEGIN_REFLECT(FRenderSettings)
		REFLECT_PROPERTY("Strength", mStrength)
		(META_DATA(EPropertyMetaData_Min, 0.0f)) REFLECT_PROPERTY("FilterRadius", mFilterRadius)
			REFLECT_PROPERTY("Threshold", mThreshold) REFLECT_PROPERTY("ProcessMode", mProcessMode);
	}

	REFLECT(SceneRenderer)
	{
		BEGIN_REFLECT(SceneRenderer)
		REFLECT_PROPERTY("RenderSettings", mRenderSettings)
		REFLECT_PROPERTY("RenderFlags", mFlags)(META_DATA(EPropertyMetaData_Flags, EPropertyFlags_BitFlags));
	}

	REFLECT(ESceneRendererFlags)
	{
		BEGIN_REFLECT_ENUM(ESceneRendererFlags)
		(ENUM_VALUE(ESceneRendererFlags_NoShadows, "NoShadows"),
		 ENUM_VALUE(ESceneRendererFlags_VisualizeColliders, "VisualizeColliders"));
	}
} // namespace BHive