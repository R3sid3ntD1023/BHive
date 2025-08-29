#include "core/math/Frustum.h"
#include "gfx/Framebuffer.h"
#include "gfx/RenderCommand.h"
#include "gfx/Shader.h"
#include "gfx/ShaderManager.h"
#include "gfx/StorageBuffer.h"
#include "material/Material.h"
#include "Renderer.h"
#include "ShadowRenderer.h"

#define SHADOW_SSBO_BINDING 5
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
		{{1, 0, 0}, {0, -1, 0}}, {{-1, 0, 0}, {0, -1, 0}}, {{0, 1, 0}, {0, 0, 1}}, {{0, -1, 0}, {0, 0, -1}}, {{0, 0, 1}, {0, -1, 0}}, {{0, 0, -1}, {0, -1, 0}},
	};

	struct FShadowCubeSSBO
	{
		glm::mat4 ShadowViewProjections[6];
		alignas(16) glm::vec2 ShadowNearFar;
	};

	struct FShadowData
	{
		glm::uvec4 NumShadowMaps = {0, 0, 0, 0}; // {Dir, Point, Spot}
		std::array<glm::mat4, MAX_LIGHTS> DirProjections = {};
		std::array<FShadowCubeSSBO, MAX_LIGHTS> PointShadowInfos = {};
		std::array<glm::mat4, MAX_LIGHTS> SpotProjections = {};
	};

	// 0 = dir, 1 = point, 2 = spot
	struct FShadowPasses
	{
		std::array<Ref<Shader>, 3> Shaders;
		std::array<Ref<Framebuffer>, 3> FBOs;
	};

	struct FShadowRenderData
	{
		FShadowPasses ShadowPasses;
		Ref<StorageBuffer> ShadowBuffer;
		FShadowData ShadowData;
	};

	void ShadowRenderer::Init(uint32_t max_lights, uint32_t cascaded_levels)
	{
		mShadowRenderData = CreateRef<FShadowRenderData>();

		FramebufferSpecification dir_shadow_fbo_spec{.Width = DIRECTIONAL_SHADOWMAP_SIZE, .Height = DIRECTIONAL_SHADOWMAP_SIZE, .Depth = max_lights};
		FramebufferSpecification spot_shadow_fbo_spec{.Width = SPOT_SHADOWMAP_SIZE, .Height = SPOT_SHADOWMAP_SIZE, .Depth = max_lights};
		FramebufferSpecification point_shadow_fbo_spec{.Width = POINT_SHADOWMAP_SIZE, .Height = POINT_SHADOWMAP_SIZE, .Depth = max_lights * 6};

		FTextureCreateInfo shadow_texture_specs{
			.InternalFormat = EFormat::DEPTH_COMPONENT_32F,
			.WrapMode = EWrapMode::CLAMP_TO_EDGE,
			.CompareMode = ETextureCompareMode::COMPARE_REF_TO_TEXTURE,
			.CompareFunc = ETextureCompareFunc::LEQUAL};

		dir_shadow_fbo_spec.Attachments.attach(shadow_texture_specs, ETextureType::TEXTURE_2D_ARRAY);
		point_shadow_fbo_spec.Attachments.attach(shadow_texture_specs, ETextureType::TEXTURE_CUBE_MAP_ARRAY);
		spot_shadow_fbo_spec.Attachments.attach(shadow_texture_specs, ETextureType::TEXTURE_2D_ARRAY);

		auto &shadow_passes = mShadowRenderData->ShadowPasses;
		shadow_passes.FBOs[0] = CreateRef<Framebuffer>(dir_shadow_fbo_spec);
		shadow_passes.FBOs[1] = CreateRef<Framebuffer>(point_shadow_fbo_spec);
		shadow_passes.FBOs[2] = CreateRef<Framebuffer>(spot_shadow_fbo_spec);
		shadow_passes.Shaders[0] = ShaderManager::Get().Load(ENGINE_SHADER_PATH "/shadow_passes/ShadowDirectionalLight.glsl");
		shadow_passes.Shaders[1] = ShaderManager::Get().Load(ENGINE_SHADER_PATH "/shadow_passes/ShadowPointLight.glsl");
		shadow_passes.Shaders[2] = ShaderManager::Get().Load(ENGINE_SHADER_PATH "/shadow_passes/ShadowSpotLight.glsl");

		mShadowRenderData->ShadowBuffer = CreateRef<StorageBuffer>(sizeof(FShadowData));
	}

	void ShadowRenderer::Begin()
	{
		mShadowRenderData->ShadowData.NumShadowMaps = {0, 0, 0, 0};
	}

	void ShadowRenderer::End()
	{

		mShadowRenderData->ShadowBuffer->BindBufferBase(SHADOW_SSBO_BINDING);
		mShadowRenderData->ShadowBuffer->SetData(&mShadowRenderData->ShadowData, sizeof(FShadowData));
	}

	void ShadowRenderer::Render(const FMeshRenderDatas &datas)
	{
		const auto &num_shadow_maps = glm::compAdd(mShadowRenderData->ShadowData.NumShadowMaps);
		if (num_shadow_maps == 0)
			return;

		RenderCommand::CullFront();

		auto draw_meshes = [=]()
		{
			for (auto &[dist, obj] : datas)
			{
				Renderer::SubmitMesh(obj);
			}
		};

		if (mShadowRenderData->ShadowData.NumShadowMaps.x > 0)
		{
			mShadowRenderData->ShadowPasses.FBOs[0]->Bind();

			RenderCommand::Clear(Buffer_Depth);

			mShadowRenderData->ShadowPasses.Shaders[0]->Bind();

			draw_meshes();

			mShadowRenderData->ShadowPasses.FBOs[0]->UnBind();
		}

		if (mShadowRenderData->ShadowData.NumShadowMaps.y > 0)
		{
			mShadowRenderData->ShadowPasses.FBOs[1]->Bind();

			RenderCommand::Clear(Buffer_Depth);

			mShadowRenderData->ShadowPasses.Shaders[1]->Bind();

			draw_meshes();

			mShadowRenderData->ShadowPasses.Shaders[1]->UnBind();
		}

		if (mShadowRenderData->ShadowData.NumShadowMaps.z > 0)
		{
			mShadowRenderData->ShadowPasses.FBOs[2]->Bind();

			RenderCommand::Clear(Buffer_Depth);

			mShadowRenderData->ShadowPasses.Shaders[2]->Bind();

			draw_meshes();

			mShadowRenderData->ShadowPasses.Shaders[2]->UnBind();
		}

		RenderCommand::CullBack();
	}

	void ShadowRenderer::SubmitDirectionalLight(const FShadowCascadedCreateInfo &info)
	{

		auto frustum = FrustumViewer(info.CameraProj, info.InverseCameraView);
		auto center = frustum.GetPosition();

		const auto light_view = glm::lookAt({}, info.LightDirection, {0, 1, 0});

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

		auto &shadow_data = mShadowRenderData->ShadowData;
		auto k = shadow_data.NumShadowMaps.x % MAX_LIGHTS;
		auto projection = glm::ortho<float>(min_x, max_x, min_y, max_y, min_z, max_z);
		shadow_data.DirProjections[k] = projection * light_view;
		shadow_data.NumShadowMaps.x++;
	}

	void ShadowRenderer::SubmitSpotLight(const FShadowFrustumCreateInfo &info)
	{
		auto view = glm::lookAt(info.LightPosition, info.LightPosition + info.LightDirection, {0, 1, 0});
		auto proj = glm::perspective<float>(glm::radians(info.LightAngleNearFar.x), 1.f, info.LightAngleNearFar.y, info.LightAngleNearFar.z);

		auto &shadow_data = mShadowRenderData->ShadowData;
		auto k = shadow_data.NumShadowMaps.z % MAX_LIGHTS;
		shadow_data.SpotProjections[k] = proj * view;
		shadow_data.NumShadowMaps.z++;
	}

	void ShadowRenderer::SubmitPointLight(const FShadowCubeCreateInfo &info)
	{
		auto &shadow_data = mShadowRenderData->ShadowData;

		auto proj = glm::perspective(glm::radians(90.0f), 1.f, info.LightNearFar.x, info.LightNearFar.y);
		auto i = (shadow_data.NumShadowMaps.y % MAX_LIGHTS);

		for (int j = 0; j < 6; j++)
		{
			auto view = glm::lookAt(info.LightPosition, info.LightPosition + point_directions[j].normal, point_directions[j].up);
			shadow_data.PointShadowInfos[i].ShadowViewProjections[j] = proj * view;
		}

		shadow_data.PointShadowInfos[i].ShadowNearFar = info.LightNearFar;
		shadow_data.NumShadowMaps.y++;
	}

	void ShadowRenderer::BindShadowMaps(uint32_t *bindings)
	{
		if (bindings)
		{
			auto &fbos = mShadowRenderData->ShadowPasses.FBOs;
			fbos[0]->GetDepthAttachment()->Bind(bindings[0]);
			fbos[1]->GetDepthAttachment()->Bind(bindings[1]);
			fbos[2]->GetDepthAttachment()->Bind(bindings[2]);
		}
	}
} // namespace BHive