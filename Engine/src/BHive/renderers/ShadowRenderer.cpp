#include "ShadowRenderer.h"
#include "gfx/Framebuffer.h"
#include "gfx/UniformBuffer.h"
#include "gfx/RenderCommand.h"
#include "gfx/Shader.h"
#include "gfx/Camera.h"
#include "Lights.h"

namespace BHive
{
	struct LightDirections
	{
		glm::vec3 normal;
		glm::vec3 up;
	};

	static LightDirections point_directions[] =
		{
			{{1, 0, 0}, {0, -1, 0}},
			{{-1, 0, 0}, {0, -1, 0}},
			{{0, 1, 0}, {0, 0, 1}},
			{{0, -1, 0}, {0, 0, -1}},
			{{0, 0, 1}, {0, -1, 0}},
			{{0, 0, -1}, {0, -1, 0}},
	};

	struct ShadowBuffersData
	{
		Ref<Shader> mShadowPassShader;
		Ref<Shader> mSpotShadowPassShader;
		Ref<Shader> mPointShadowPassShader;

		Ref<Framebuffer> mShadowPassFBO;
		Ref<Framebuffer> mShadowSpotPassFBO;
		Ref<Framebuffer> mPointShadowPassFBO;

		Ref<UniformBuffer> mShadowBuffer;
		Ref<UniformBuffer> mSpotShadowBuffer;
		Ref<UniformBuffer> mPointShadowBuffer;

		std::vector<glm::mat4> mLightViewProjections;
		std::vector<glm::mat4> mSpotLightViewProjections;
		std::vector<glm::mat4> mPointLightViewProjections;

		uint32_t mNumDirectionalLights = 0;
		uint32_t mNumSpotLights = 0;
		uint32_t mNumPointLights = 0;
	};

	static ShadowBuffersData mShadowRenderData;
	static uint32_t sDirectionalShadowMapSize = 2048;
	static uint32_t sPointShadowMapSize = 256;
	static uint32_t sSpotShadowMapSize = 512;

	void ShadowRenderer::Init(uint32_t max_lights, uint32_t cascaded_levels)
	{
		FramebufferSpecification shadow_fbo_specs{};
		shadow_fbo_specs.Width = sDirectionalShadowMapSize;
		shadow_fbo_specs.Height = sDirectionalShadowMapSize;
		shadow_fbo_specs.Depth = max_lights;
		shadow_fbo_specs.Attachments.attach({
												.Format = EFormat::DEPTH_COMPONENT_32F,
												.WrapMode = EWrapMode::CLAMP_TO_EDGE,
												.BorderColor = {0, 0, 0, 1.f},
												.Type = ETextureType::TEXTURE_ARRAY_2D,
												.CompareMode = ETextureCompareMode::COMPARE_REF_TO_TEXTURE,
												.CompareFunc = ETextureCompareFunc::LEQUAL,

											},
											ETextureType::TEXTURE_3D);

		mShadowRenderData.mShadowPassFBO = Framebuffer::Create(shadow_fbo_specs);

		shadow_fbo_specs.Width = sSpotShadowMapSize;
		shadow_fbo_specs.Height = sSpotShadowMapSize;
		shadow_fbo_specs.Depth = max_lights;
		shadow_fbo_specs.Attachments.attach({
												.Format = EFormat::DEPTH_COMPONENT_32F,
												.WrapMode = EWrapMode::CLAMP_TO_EDGE,
												.Type = ETextureType::TEXTURE_ARRAY_2D,
												.CompareMode = ETextureCompareMode::COMPARE_REF_TO_TEXTURE,
												.CompareFunc = ETextureCompareFunc::LEQUAL,

											},
											ETextureType::TEXTURE_3D);

		mShadowRenderData.mShadowSpotPassFBO = Framebuffer::Create(shadow_fbo_specs);

		shadow_fbo_specs.Width = sPointShadowMapSize;
		shadow_fbo_specs.Height = sPointShadowMapSize;
		shadow_fbo_specs.Depth = max_lights * 6;
		shadow_fbo_specs.Attachments.reset().attach({

														.Format = EFormat::DEPTH_COMPONENT_32F,
														.WrapMode = EWrapMode::CLAMP_TO_BORDER,
														.BorderColor = {0.f, 0.f, 0.f, 0.f},
														.Type = ETextureType::TEXTURE_CUBE_MAP_ARRAY,
														.CompareMode = ETextureCompareMode::COMPARE_REF_TO_TEXTURE,
														.CompareFunc = ETextureCompareFunc::LEQUAL,

													},
													ETextureType::TEXTURE_3D);
		mShadowRenderData.mPointShadowPassFBO = Framebuffer::Create(shadow_fbo_specs);

		mShadowRenderData.mShadowBuffer = UniformBuffer::Create(2, sizeof(uint32_t) * 4 + sizeof(glm::mat4) * max_lights);
		mShadowRenderData.mSpotShadowBuffer = UniformBuffer::Create(3, sizeof(uint32_t) * 4 + sizeof(glm::mat4) * max_lights);
		mShadowRenderData.mPointShadowBuffer = UniformBuffer::Create(4, sizeof(uint32_t) * 4 + sizeof(glm::mat4) * max_lights * 6);

		mShadowRenderData.mLightViewProjections.resize(max_lights);
		mShadowRenderData.mSpotLightViewProjections.resize(max_lights);
		mShadowRenderData.mPointLightViewProjections.resize(max_lights * 6);

		mShadowRenderData.mShadowPassShader = ShaderLibrary::Load(ENGINE_PATH "/data/shaders/ShadowPass.glsl");
		mShadowRenderData.mSpotShadowPassShader = ShaderLibrary::Load(ENGINE_PATH "/data/shaders/SpotShadowPass.glsl");
		mShadowRenderData.mPointShadowPassShader = ShaderLibrary::Load(ENGINE_PATH "/data/Shaders/ShadowPointPass.glsl");
	}

	void ShadowRenderer::Begin()
	{
		mShadowRenderData.mNumDirectionalLights = 0;
		mShadowRenderData.mNumSpotLights = 0;
		mShadowRenderData.mNumPointLights = 0;

		RenderCommand::CullFront();
	}

	void ShadowRenderer::End()
	{
		RenderCommand::CullBack();
	}

	void ShadowRenderer::BeginShadowPass()
	{
		mShadowRenderData.mShadowPassFBO->Bind();
		RenderCommand::Clear(Buffer_Depth);
		mShadowRenderData.mShadowPassShader->Bind();
	}

	void ShadowRenderer::EndShadowPass()
	{
		mShadowRenderData.mShadowPassShader->UnBind();
		mShadowRenderData.mShadowPassFBO->UnBind();
	}

	void ShadowRenderer::BeginSpotShadowPass()
	{
		mShadowRenderData.mShadowSpotPassFBO->Bind();
		RenderCommand::Clear(Buffer_Depth);

		mShadowRenderData.mSpotShadowPassShader->Bind();
	}

	void ShadowRenderer::EndSpotShadowPass()
	{
		mShadowRenderData.mSpotShadowPassShader->UnBind();
		mShadowRenderData.mShadowSpotPassFBO->UnBind();
	}

	void ShadowRenderer::BeginPointShadowPass()
	{
		mShadowRenderData.mPointShadowPassFBO->Bind();
		RenderCommand::Clear(Buffer_Depth);

		mShadowRenderData.mPointShadowPassShader->Bind();
	}

	void ShadowRenderer::EndPointShadowPass()
	{
		mShadowRenderData.mPointShadowPassShader->UnBind();
		mShadowRenderData.mPointShadowPassFBO->UnBind();
	}

	void ShadowRenderer::SubmitDirectionalLight(const glm::vec3 &direction, const glm::mat4 &camera_proj,
												const glm::mat4 &camera_view)
	{
		// auto frustum_corners = Camera::GetFrustumCorners(camera_proj, camera_view);
		// auto center = Camera::GetFrustumCenter(frustum_corners);

		const auto view = glm::lookAt({}, direction, {0, 1, 0});

		/*float min_x = std::numeric_limits<float>::max();
		float max_x = std::numeric_limits<float>::lowest();
		float min_y = std::numeric_limits<float>::max();
		float max_y = std::numeric_limits<float>::lowest();
		float min_z = std::numeric_limits<float>::max();
		float max_z = std::numeric_limits<float>::lowest();

		for (const auto& v : frustum_corners)
		{
			const auto trf = view * v;
			min_x = std::min(min_x, trf.x);
			max_x = std::max(max_x, trf.x);
			min_y = std::min(min_y, trf.y);
			max_y = std::max(max_y, trf.y);
			min_z = std::min(min_z, trf.z);
			max_z = std::max(max_z, trf.z);
		}

		constexpr float z_multi = 10.0f;
		if (min_z < 0)
		{
			min_z *= z_multi;
		}
		else
		{
			min_z /= z_multi;
		}
		if (max_z < 0)
		{
			max_z /= z_multi;
		}
		else
		{
			max_z *= z_multi;
		}*/

		auto k = mShadowRenderData.mNumDirectionalLights % MAX_LIGHTS;
		auto projection = glm::ortho<float>(-30, 30, -30, 30, -30.f, 100.f);
		mShadowRenderData.mLightViewProjections[k] = projection * view;

		mShadowRenderData.mNumDirectionalLights++;
		mShadowRenderData.mShadowBuffer->SetData(mShadowRenderData.mNumDirectionalLights);
		mShadowRenderData.mShadowBuffer->SetData(mShadowRenderData.mLightViewProjections, sizeof(uint32_t) * 4);
	}

	void ShadowRenderer::SubmitSpotLight(const glm::vec3 &direction, const glm::vec3 &position)
	{
		auto view = glm::lookAt(position, position + direction, {0, 1, 0});
		auto projection = glm::perspective<float>(glm::radians(120.f), 1.f, .1f, 50.0f);

		auto k = mShadowRenderData.mNumSpotLights % MAX_LIGHTS;
		mShadowRenderData.mSpotLightViewProjections[k] = projection * view;

		mShadowRenderData.mNumSpotLights++;
		mShadowRenderData.mSpotShadowBuffer->SetData(mShadowRenderData.mNumSpotLights);
		mShadowRenderData.mSpotShadowBuffer->SetData(mShadowRenderData.mSpotLightViewProjections, sizeof(uint32_t) * 4);
	}

	void ShadowRenderer::SubmitPointLight(const glm::vec3 &position)
	{
		auto proj = glm::perspective(glm::radians(90.0f), 1.f, 0.1f, 50.f);

		for (int j = 0; j < 6; j++)
		{
			auto view = glm::lookAt(position, position + point_directions[j].normal, point_directions[j].up);

			auto k = ((mShadowRenderData.mNumPointLights % MAX_LIGHTS) * 6) + j;
			mShadowRenderData.mPointLightViewProjections[k] = proj * view;
		}

		mShadowRenderData.mNumPointLights++;
		mShadowRenderData.mPointShadowBuffer->SetData(mShadowRenderData.mNumPointLights);
		mShadowRenderData.mPointShadowBuffer->SetData(mShadowRenderData.mPointLightViewProjections, sizeof(uint32_t) * 4);
	}

	Ref<Framebuffer> ShadowRenderer::GetShadowFBO()
	{
		return mShadowRenderData.mShadowPassFBO;
	}

	Ref<Framebuffer> ShadowRenderer::GetSpotShadowFBO()
	{
		return mShadowRenderData.mShadowSpotPassFBO;
	}

	Ref<Framebuffer> ShadowRenderer::GetPointShadowFBO()
	{
		return mShadowRenderData.mPointShadowPassFBO;
	}
}