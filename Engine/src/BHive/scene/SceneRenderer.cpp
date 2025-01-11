#include "SceneRenderer.h"
#include "gfx/RenderCommand.h"
#include "gfx/VertexArray.h"
#include "gfx/Shader.h"
#include "gfx/Texture.h"
#include "gfx/UniformBuffer.h"
#include "renderers/ShadowRenderer.h"
#include <glad/glad.h>
#include "mesh/StaticMesh.h"
#include "mesh/SkeletalMesh.h"
#include "mesh/SkeletalPose.h"
#include "debug/Instrumentor.h"

#include "importers/TextureImporter.h"
#include "renderers/HDRConverter.h"

#define DRAW_ELEMENTS()                                           \
	RenderCommand::DrawElementsBaseVertex(EDrawMode::Triangles,   \
										  *vao,                   \
										  sub_mesh->mStartVertex, \
										  sub_mesh->mStartIndex,  \
										  sub_mesh->mIndexCount);

#define RENDER_SHADOWS

namespace BHive
{
	uint32_t quad_indices[] = {0, 1, 2, 2, 3, 0};
	float quad_vertices[] = {
		-1.f, -1.f, 0.f, 0.f,
		1.f, -1.f, 1.f, 0.f,
		1.f, 1.f, 1.f, 1.f,
		-1.f, 1.f, 0.f, 1.f};

	SceneRenderer::SceneRenderer(uint32_t width, uint32_t height, uint32_t flags)
		: mViewportSize(width, height), mFlags(flags)
	{
		FramebufferSpecification fbspec{};

		fbspec.Attachments
			.attach({.mFormat = EFormat::RGBA32F, .mWrapMode = EWrapMode::CLAMP_TO_EDGE})
			.attach({.mFormat = EFormat::DEPTH24_STENCIL8, .mWrapMode = EWrapMode::CLAMP_TO_EDGE});

		fbspec.Width = width;
		fbspec.Height = height;
		fbspec.Samples = 16;
		mMultiSampleFramebuffer = Framebuffer::Create(fbspec);

		fbspec.Samples = 1;
		mFramebuffer = Framebuffer::Create(fbspec);

		fbspec.Attachments.reset()
			.attach({.mFormat = EFormat::RGBA32F, .mWrapMode = EWrapMode::CLAMP_TO_EDGE});

		fbspec.Samples = 1;
		mQuadFramebuffer = Framebuffer::Create(fbspec);

		auto ibo = IndexBuffer::Create(quad_indices, 6);
		auto vbo = VertexBuffer::Create(quad_vertices, 16 * sizeof(float));
		vbo->SetLayout({{EShaderDataType::Float2}, {EShaderDataType::Float2}});

		mQuadVao = VertexArray::Create();
		mQuadVao->AddVertexBuffer(vbo);
		mQuadVao->SetIndexBuffer(ibo);

		mCube = StaticMesh::CreateSphere(300.f);

		mQuadShader = ShaderLibrary::Load(ENGINE_PATH "/data/shaders/ScreenQuad.glsl");

		mPreFilterShader = ShaderLibrary::Load(ENGINE_PATH "/data/shaders/PreFilter.glsl");

		mDownSamplerShader = ShaderLibrary::Load(ENGINE_PATH "/data/shaders/DownSample.glsl");

		mUpSamplerShader = ShaderLibrary::Load(ENGINE_PATH "/data/shaders/UpSample.glsl");

		mSkyBoxShader = ShaderLibrary::Load(ENGINE_PATH "/data/shaders/SkyBox.glsl");

		mBloomMipMaps.resize(5);
		CreateBloomMipMaps();

		auto environment_texture = TextureImporter::Import(ENGINE_PATH "/data/hdr/industrial_sunset_puresky_2k.hdr");
		HDRConverter::Get().SetEnvironmentMap(environment_texture);

		mSceneData.mOpaqueObjects.reserve(100);
		mSceneData.mTransparentObjects.reserve(100);
		mSceneData.mShadowCasters.reserve(100);

		// HDRConverter::Get().GetPreFilteredEnvironmentTetxure()->Bind(0);
		// HDRConverter::Get().GetIrradianceFBO()->GetColorAttachment()->Bind(1);
		// HDRConverter::Get().GetBDRFLUT()->Bind(2);

		if ((flags & ESceneRendererFlags_NoShadows) == 0)
		{
			ShadowRenderer::GetShadowFBO()->GetDepthAttachment()->Bind(3);
			ShadowRenderer::GetSpotShadowFBO()->GetDepthAttachment()->Bind(4);
			ShadowRenderer::GetPointShadowFBO()->GetDepthAttachment()->Bind(5);
		}
	}

	SceneRenderer::~SceneRenderer()
	{
	}

	void SceneRenderer::Begin(const glm::mat4 &projection, const glm::mat4 &view)
	{
		BH_PROFILE_FUNCTION();

		mSceneData.mProjection = projection;
		mSceneData.mView = view;
		mSceneData.clear();

		Renderer::Begin(mSceneData.mProjection, mSceneData.mView);
	}

	void SceneRenderer::End()
	{
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
		RenderCommand::DrawElementsBaseVertex(EDrawMode::Triangles, *mCube->GetVertexArray(),
											  submesh.mStartVertex, submesh.mStartIndex, submesh.mIndexCount);

		mSkyBoxShader->UnBind();

		RenderCommand::CullBack();

		RenderCommand::DepthFunc(GL_LESS);

		// RENDER OPAQUE OBJECTS SORTED

		auto &opaque = mSceneData.mOpaqueObjects;
		std::sort(opaque.begin(), opaque.end(), [this](const FObjectMaterialData &a, const FObjectMaterialData &b)
				  {
			float dist_a = glm::distance(glm::vec3(mSceneData.mView[3]), a.mObjectData.mTransform.get_translation());
			float dist_b = glm::distance(glm::vec3(mSceneData.mView[3]), b.mObjectData.mTransform.get_translation());

			return dist_a > dist_b; });

		for (auto &[material, objectdata] : opaque)
		{
			auto &[transform, sub_mesh, vao, joints] = objectdata;

			auto shader = material->Submit();
			shader->SetUniform("u_global_flags", mFlags);
			if (joints.size())
			{
				Renderer::SubmitSkeletalMesh(joints);
			}

			Renderer::SubmitTransform(transform);
			DRAW_ELEMENTS();
		}

		RenderCommand::EnableBlend(true);

		// RENDER TRANSPARENT OBJECTS SORTED

		auto &transparent = mSceneData.mTransparentObjects;
		std::sort(transparent.begin(), transparent.end(), [this](const FObjectMaterialData &a, const FObjectMaterialData &b)
				  {
			float dist_a = glm::distance(glm::vec3(mSceneData.mView[3]), a.mObjectData.mTransform.get_translation());
			float dist_b = glm::distance(glm::vec3(mSceneData.mView[3]), b.mObjectData.mTransform.get_translation());

			return dist_a < dist_b; });

		for (auto &[material, objectdata] : transparent)
		{
			auto &[transform, sub_mesh, vao, joints] = objectdata;

			auto shader = material->Submit();
			shader->SetUniform("u_global_flags", mFlags);

			if (joints.size())
			{
				Renderer::SubmitSkeletalMesh(joints);
			}

			Renderer::SubmitTransform(transform);
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
		ProcessBloom();

		// render screen quad

		mQuadFramebuffer->Bind();

		RenderCommand::DisableDepth();

		RenderCommand::ClearColor(.2f, .2f, .2f, 1.0f);
		RenderCommand::Clear(Buffer_Color);

		mQuadShader->Bind();
		mQuadShader->SetUniform("u_bloomstrength", mRenderSettings.mStrength);

		mFramebuffer->GetColorAttachment()->Bind();
		mBloomMipMaps[0]->Bind(1);
		RenderCommand::DrawElements(Triangles, *mQuadVao);

		mQuadShader->UnBind();

		mQuadFramebuffer->UnBind();

		RenderCommand::ClearColor(0, 0, 0, 1.0f);
		RenderCommand::Clear(Buffer_Color);

		mQuadShader->Bind();
		mQuadShader->SetUniform("u_postprocess_mode", mRenderSettings.mProcessMode);

		mQuadFramebuffer->GetColorAttachment()->Bind();
		RenderCommand::DrawElements(Triangles, *mQuadVao);

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

	void SceneRenderer::SubmitStaticMesh(const Ref<StaticMesh> &static_mesh, const FTransform &transform, const Ref<Material> &material)
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

	void SceneRenderer::SubmitStaticMesh(const Ref<StaticMesh> &static_mesh, const FTransform &transform, const MaterialTable &materials)
	{
		if (!static_mesh)
			return;

		auto vao = static_mesh->GetVertexArray();
		for (auto &sub_mesh : *static_mesh)
		{
			auto material = materials.get_material(sub_mesh.mMaterialIndex).get();
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

	void SceneRenderer::SubmitSkeletalMesh(const Ref<SkeletalMesh> &skeletal_mesh, const FTransform &transform,
										   const std::vector<glm::mat4> &joints, const MaterialTable &materials)
	{
		if (!skeletal_mesh)
			return;

		auto vao = skeletal_mesh->GetVertexArray();
		for (auto &sub_mesh : *skeletal_mesh)
		{
			auto material = materials.get_material(sub_mesh.mMaterialIndex).get();

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

		CreateBloomMipMaps();
	}

	const Ref<Framebuffer> &SceneRenderer::GetFramebuffer() const
	{
		return mFramebuffer;
	}

	const Ref<Framebuffer> &SceneRenderer::GetFinalFramebuffer() const
	{
		return mQuadFramebuffer;
	}

	void SceneRenderer::CreateBloomMipMaps()
	{
		FTextureSpecification specs{};
		specs.mFormat = EFormat::R11_G11_B10;
		specs.mWrapMode = EWrapMode::CLAMP_TO_BORDER;

		for (auto &mip : mBloomMipMaps)
			mip.reset();

		mPreFilterTexture.reset();

		mPreFilterTexture = Texture2D::Create(nullptr, mViewportSize.x, mViewportSize.y, specs);

		glm::ivec2 mps = mViewportSize;
		for (auto &mip : mBloomMipMaps)
		{
			mip = Texture2D::Create(nullptr, mps.x, mps.y, specs);

			mps /= 2;
			if (mps.x < 1)
				mps.x = 1;
			if (mps.y < 1)
				mps.y = 1;
		}
	}

	void SceneRenderer::ProcessBloom()
	{
		// get bloom filetered
		mPreFilterShader->Bind();
		mPreFilterShader->SetUniform("u_threshold", mRenderSettings.mThreshold);

		mFramebuffer->GetColorAttachment()->Bind();
		mPreFilterTexture->BindAsImage(0, GL_WRITE_ONLY);
		mPreFilterShader->Dispatch(mPreFilterTexture->GetWidth(), mPreFilterTexture->GetHeight());

		mPreFilterShader->UnBind();

		// downsample
		mDownSamplerShader->Bind();

		auto texture = mPreFilterTexture;
		for (auto &mip : mBloomMipMaps)
		{
			texture->Bind();
			mip->BindAsImage(0, GL_WRITE_ONLY);

			glm::ivec2 size = {mip->GetWidth(), mip->GetHeight()};
			mDownSamplerShader->Dispatch(size.x, size.y);

			texture = mip;
		}
		mDownSamplerShader->UnBind();

		mUpSamplerShader->Bind();
		mUpSamplerShader->SetUniform("u_filterRadius", mRenderSettings.mFilterRadius);

		for (size_t i = mBloomMipMaps.size() - 1; i > 0; i--)
		{
			const auto &mip = mBloomMipMaps[i];
			const auto &next_mip = mBloomMipMaps[i - 1];

			mip->Bind();
			next_mip->BindAsImage(0, GL_READ_WRITE);
			mUpSamplerShader->Dispatch(next_mip->GetWidth(), next_mip->GetHeight());
		}

		mUpSamplerShader->UnBind();
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

				Renderer::SubmitTransform(transform);

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

				Renderer::SubmitTransform(transform);
				DRAW_ELEMENTS();
			}

			ShadowRenderer::EndPointShadowPass();
		}

		if (render_spot_shadows)
		{
			for (auto &light : spot_lights)
			{
				auto &transform = light.mTransform;
				ShadowRenderer::SubmitSpotLight(transform.get_forward(), transform.get_translation(), Cast<SpotLight>(light.mLight)->mRadius);
			}

			ShadowRenderer::BeginSpotShadowPass();

			for (auto &object : mSceneData.mShadowCasters)
			{
				auto &[transform, sub_mesh, vao, joints] = object;
				if (joints.size())
				{
					Renderer::SubmitSkeletalMesh(joints);
				}

				Renderer::SubmitTransform(transform);
				DRAW_ELEMENTS();
			}

			ShadowRenderer::EndSpotShadowPass();

			ShadowRenderer::End();
		}
	}

	REFLECT(EPostProcessMode)
	{
		BEGIN_REFLECT_ENUM(EPostProcessMode)
		(
			ENUM_VALUE(EPostProcessMode_None),
			ENUM_VALUE(EPostProcessMode_ACES));
	}

	REFLECT(FRenderSettings)
	{
		BEGIN_REFLECT(FRenderSettings)
		REFLECT_PROPERTY("Strength", mStrength)
		(META_DATA(EPropertyMetaData_Min, 0.0f))
			REFLECT_PROPERTY("FilterRadius", mFilterRadius)
				REFLECT_PROPERTY("Threshold", mThreshold)
					REFLECT_PROPERTY("ProcessMode", mProcessMode);
	}

	REFLECT(SceneRenderer)
	{
		BEGIN_REFLECT(SceneRenderer)
		REFLECT_PROPERTY("RenderSettings", mRenderSettings);
	}
}