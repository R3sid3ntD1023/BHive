#include "gfx/Camera.h"
#include "gfx/Framebuffer.h"
#include "gfx/RenderCommand.h"
#include "gfx/Shader.h"
#include "gfx/ShaderManager.h"
#include "gfx/StorageBuffer.h"
#include "gfx/UniformBuffer.h"
#include "Lights.h"
#include "LineRenderer.h"
#include "core/math/Frustum.h"
#include "ShadowRenderer.h"

#define DIRECTIONAL_SHADOWMAP_SIZE 1024
#define POINT_SHADOWMAP_SIZE 1024
#define SPOT_SHADOWMAP_SIZE 512

namespace BHive
{
	struct LightDirections
	{
		glm::vec3 normal;
		glm::vec3 up;
	};

	static LightDirections point_directions[] = {
		{{1, 0, 0}, {0, -1, 0}},  {{-1, 0, 0}, {0, -1, 0}}, {{0, 1, 0}, {0, 0, 1}},
		{{0, -1, 0}, {0, 0, -1}}, {{0, 0, 1}, {0, -1, 0}},	{{0, 0, -1}, {0, -1, 0}},
	};

	struct FShadowData
	{
		glm::uvec4 NumShadowMaps = {0, 0, 0, 0}; // {Dir, Point, Spot}
		std::array<glm::mat4, MAX_LIGHTS> DirProjections = {};
		std::array<glm::mat4, MAX_LIGHTS * 6> PointProjections = {};
		std::array<glm::mat4, MAX_LIGHTS> SpotProjections = {};
	};

	struct ShadowBuffersData
	{
		Ref<Shader> mShadowPassShader;
		Ref<Shader> mSpotShadowPassShader;
		Ref<Shader> mPointShadowPassShader;

		Ref<Framebuffer> mShadowPassFBO;
		Ref<Framebuffer> mShadowSpotPassFBO;
		Ref<Framebuffer> mPointShadowPassFBO;

		Ref<StorageBuffer> mShadowBuffer;
		uint32_t ShadowSSBOBinding = 0;
		FShadowData mShadowData;
	};

	static ShadowBuffersData mShadowRenderData;

	void ShadowRenderer::Init(uint32_t max_lights, uint32_t cascaded_levels)
	{
		FramebufferSpecification shadow_fbo_specs{};
		shadow_fbo_specs.Width = DIRECTIONAL_SHADOWMAP_SIZE;
		shadow_fbo_specs.Height = DIRECTIONAL_SHADOWMAP_SIZE;
		shadow_fbo_specs.Depth = max_lights;
		shadow_fbo_specs.Attachments.attach(
			{
				.InternalFormat = EFormat::DEPTH_COMPONENT_32F,
				.WrapMode = EWrapMode::CLAMP_TO_EDGE,
				.BorderColor = {0, 0, 0, 1.f},
				.CompareMode = ETextureCompareMode::COMPARE_REF_TO_TEXTURE,
				.CompareFunc = ETextureCompareFunc::LEQUAL,

			},
			ETextureType::TEXTURE_2D_ARRAY);

		mShadowRenderData.mShadowPassFBO = CreateRef<Framebuffer>(shadow_fbo_specs);

		shadow_fbo_specs.Width = SPOT_SHADOWMAP_SIZE;
		shadow_fbo_specs.Height = SPOT_SHADOWMAP_SIZE;
		shadow_fbo_specs.Depth = max_lights;
		shadow_fbo_specs.Attachments.reset()
			.attach(
				{
					.InternalFormat = EFormat::RG32F,
					.WrapMode = EWrapMode::CLAMP_TO_EDGE,
				},
				ETextureType::TEXTURE_2D_ARRAY)
			.attach(
				{
					.InternalFormat = EFormat::DEPTH_COMPONENT_32F,
					.WrapMode = EWrapMode::CLAMP_TO_EDGE,
					.CompareMode = ETextureCompareMode::COMPARE_REF_TO_TEXTURE,
					.CompareFunc = ETextureCompareFunc::LEQUAL,

				},
				ETextureType::TEXTURE_2D_ARRAY);

		mShadowRenderData.mShadowSpotPassFBO = CreateRef<Framebuffer>(shadow_fbo_specs);

		shadow_fbo_specs.Width = POINT_SHADOWMAP_SIZE;
		shadow_fbo_specs.Height = POINT_SHADOWMAP_SIZE;
		shadow_fbo_specs.Depth = max_lights * 6;
		shadow_fbo_specs.Attachments.reset()
			.attach(
				{.InternalFormat = EFormat::RG32F, .WrapMode = EWrapMode::CLAMP_TO_BORDER, .BorderColor = 0xffffffff},
				ETextureType::TEXTURE_CUBE_MAP_ARRAY)
			.attach(
				{
					.InternalFormat = EFormat::DEPTH_COMPONENT_32F,
					.WrapMode = EWrapMode::CLAMP_TO_EDGE,
					.CompareMode = ETextureCompareMode::COMPARE_REF_TO_TEXTURE,
					.CompareFunc = ETextureCompareFunc::LEQUAL,
				},
				ETextureType::TEXTURE_2D_ARRAY);
		mShadowRenderData.mPointShadowPassFBO = CreateRef<Framebuffer>(shadow_fbo_specs);

		mShadowRenderData.mShadowBuffer = CreateRef<StorageBuffer>(sizeof(FShadowData));

		mShadowRenderData.mShadowPassShader = ShaderManager::Get().Load(ENGINE_PATH "/data/shaders/ShadowPass.glsl");
		mShadowRenderData.mSpotShadowPassShader = ShaderManager::Get().Load(ENGINE_PATH "/data/shaders/SpotShadowPass.glsl");
		auto point_shadow_shader = mShadowRenderData.mPointShadowPassShader =
			ShaderManager::Get().Load(ENGINE_PATH "/data/Shaders/ShadowPointPass.glsl");

		auto &data = point_shadow_shader->GetRelectionData();
		if (data.StorageBuffers.contains("ShadowSSBO"))
		{
			mShadowRenderData.ShadowSSBOBinding = data.StorageBuffers.at("ShadowSSBO").Binding;
		}
	}

	void ShadowRenderer::Begin()
	{
		mShadowRenderData.mShadowData.NumShadowMaps = {0, 0, 0, 0};

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
		RenderCommand::Clear(Buffer_Depth | Buffer_Color);

		mShadowRenderData.mPointShadowPassShader->Bind();
	}

	void ShadowRenderer::EndPointShadowPass()
	{
		mShadowRenderData.mPointShadowPassShader->UnBind();
		mShadowRenderData.mPointShadowPassFBO->UnBind();
	}

	void ShadowRenderer::SubmitDirectionalLight(
		const glm::vec3 &direction, const glm::mat4 &camera_proj, const glm::mat4 &camera_view)
	{

		auto frustum = FrustumViewer(camera_proj, camera_view);
		auto center = frustum.GetPosition();

		const auto light_view = glm::lookAt({}, direction, {0, 1, 0});

		float min_x = std::numeric_limits<float>::max();
		float max_x = std::numeric_limits<float>::lowest();
		float min_y = std::numeric_limits<float>::max();
		float max_y = std::numeric_limits<float>::lowest();
		float min_z = std::numeric_limits<float>::max();
		float max_z = std::numeric_limits<float>::lowest();

		for (const auto &v : frustum.GetPoints())
		{
			const auto trf = light_view * v;
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
		}

		auto &shadow_data = mShadowRenderData.mShadowData;
		auto k = shadow_data.NumShadowMaps.x % MAX_LIGHTS;
		auto projection = glm::ortho<float>(min_x, max_x, min_y, max_y, min_z, max_z);
		shadow_data.DirProjections[k] = projection * light_view;

		shadow_data.NumShadowMaps.x++;

		mShadowRenderData.mShadowBuffer->BindBufferBase(mShadowRenderData.ShadowSSBOBinding);
		mShadowRenderData.mShadowBuffer->SetData(&mShadowRenderData.mShadowData, sizeof(FShadowData));
	}

	void ShadowRenderer::SubmitSpotLight(const glm::vec3 &direction, const glm::vec3 &position, float radius)
	{
		auto view = glm::lookAt(position, position + direction, {0, 1, 0});
		auto proj = glm::perspective<float>(glm::radians(120.f), 1.f, .1f, radius);

		auto &shadow_data = mShadowRenderData.mShadowData;
		auto k = shadow_data.NumShadowMaps.z % MAX_LIGHTS;
		shadow_data.SpotProjections[k] = proj * view;

		FrustumViewer viewer(proj, view);
		LineRenderer::DrawFrustum(viewer, {1, .5f, 1, 1});

		shadow_data.NumShadowMaps.z++;

		mShadowRenderData.mShadowBuffer->BindBufferBase(mShadowRenderData.ShadowSSBOBinding);
		mShadowRenderData.mShadowBuffer->SetData(&mShadowRenderData.mShadowData, sizeof(FShadowData));
	}

	void ShadowRenderer::SubmitPointLight(const glm::vec3 &position, float radius)
	{
		auto &shadow_data = mShadowRenderData.mShadowData;
		auto proj = glm::perspective(glm::radians(90.0f), 1.f, 1.f, radius);

		for (int j = 0; j < 6; j++)
		{
			auto view = glm::lookAt(position, position + point_directions[j].normal, point_directions[j].up);

			auto k = ((shadow_data.NumShadowMaps.y % MAX_LIGHTS) * 6) + j;
			shadow_data.PointProjections[k] = proj * view;

			FrustumViewer viewer(proj, view);
			LineRenderer::DrawFrustum(viewer, {1, .5f, 0, 1});
		}

		shadow_data.NumShadowMaps.y++;

		mShadowRenderData.mShadowBuffer->BindBufferBase(mShadowRenderData.ShadowSSBOBinding);
		mShadowRenderData.mShadowBuffer->SetData(&mShadowRenderData.mShadowData, sizeof(FShadowData));
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
} // namespace BHive