#include "gfx/RenderCommand.h"
#include "gfx/Shader.h"
#include "gfx/textures/Texture2D.h"
#include "gfx/UniformBuffer.h"
#include "gfx/StorageBuffer.h"
#include "Renderer.h"
#include "ShadowRenderer.h"

#include <glad/glad.h>

#define CAMERA_UBO_BINDING 0
#define LIGHT_BUFFER_BINDING 4
#define DIR_LIGHT_STORAGE_BINDING 4
#define POINT_LIGHT_STORAGE_BINDING 5
#define SPOT_LIGHT_STORAGE_BINDING 6

namespace BHive
{
	struct FDirectionalLightInfo
	{
		alignas(16) glm::vec3 Color;
		alignas(16) glm::vec3 Direction;
	};

	struct FPointLightInfo
	{
		alignas(16) glm::vec3 Color;
		alignas(16) glm::vec3 Position;
		float Radius;
	};

	struct FSpotLightInfo
	{
		alignas(16) glm::vec3 Color;
		alignas(16) glm::vec3 Position;
		alignas(16) glm::vec3 Direction;
		float Radius;
		float InnerCutoff;
		float OuterCutoff;
	};

	struct FLightInfo
	{
		uint32_t NumDirectionalLights = 0;
		uint32_t NumPointLights = 0;
		uint32_t NumSpotLights = 0;

		std::array<FDirectionalLightInfo, MAX_LIGHTS> DirectionalLightInfo;
		std::array<FPointLightInfo, MAX_LIGHTS> PointLightInfo;
		std::array<FSpotLightInfo, MAX_LIGHTS> SpotLightInfo;
	};

	struct Renderer::RenderData
	{

		Ref<UniformBuffer> LightBuffer;

		Ref<Texture> WhiteTexture;
		Ref<Texture> BlackTexture;
		Ref<Texture> BlueTexture;

		FLightInfo LightInfo{};

		RenderData()
		{
			constexpr uint32_t i = MAX_LIGHTS * sizeof(FPointLightInfo) + sizeof(uint32_t);
			LightBuffer = CreateRef<UniformBuffer>(LIGHT_BUFFER_BINDING, sizeof(FLightInfo));

			uint32_t white = 0xFFFFFFFF;
			FTextureSpecification texture_specs{};
			texture_specs.Channels = 3;
			texture_specs.InternalFormat = EFormat::RGB8;

			WhiteTexture = CreateRef<Texture2D>(1, 1, texture_specs, &white, sizeof(uint32_t));

			uint32_t black = 0xFF000000;
			BlackTexture = CreateRef<Texture2D>(1, 1, texture_specs, &black, sizeof(uint32_t));

			uint32_t blue = 0xFF0000FF;
			BlueTexture = CreateRef<Texture2D>(1, 1, texture_specs, &blue, sizeof(uint32_t));
		}
	};

	void Renderer::Init()
	{
		sData = new RenderData();

		ShadowRenderer::Init(MAX_LIGHTS);
		LineRenderer::Init();
		QuadRenderer::Init();
	}

	void Renderer::Shutdown()
	{

		LineRenderer::Shutdown();
		QuadRenderer::Shutdown();
		ShadowRenderer::Shutdown();

		delete sData;
	}

	void Renderer::Begin()
	{
		ResetStats();

		sData->LightBuffer->SetData(&sData->LightInfo, sizeof(FLightInfo));

		sData->LightInfo.NumDirectionalLights = 0;
		sData->LightInfo.NumPointLights = 0;
		sData->LightInfo.NumSpotLights = 0;

		LineRenderer::Begin();
		QuadRenderer::Begin();
	}

	void Renderer::SubmitCamera(const glm::mat4 &projection, const glm::mat4 &view)
	{
		CameraBuffer::Get().Submit(projection, view);
	}

	void Renderer::SubmitLight(const DirectionalLight &light, const glm::vec3 &direction)
	{
		auto &camera = CameraBuffer::Get().GetCameraData();

		auto num_lights = sData->LightInfo.NumDirectionalLights++ % MAX_LIGHTS;
		sData->LightInfo.DirectionalLightInfo[num_lights] = {light.Color, direction};

		ShadowRenderer::SubmitDirectionalLight(direction, camera.Projection, camera.View);
	}

	void Renderer::SubmitLight(const PointLight &light, const glm::vec3 &position)
	{
		auto num_lights = sData->LightInfo.NumPointLights++ % MAX_LIGHTS;
		sData->LightInfo.PointLightInfo[num_lights] = {light.Color, position, light.Radius};

		ShadowRenderer::SubmitPointLight(position, light.Radius);
	}

	void Renderer::SubmitLight(const SpotLight &light, const glm::vec3 &direction, const glm::vec3 &position)
	{
		auto num_lights = sData->LightInfo.NumSpotLights++ % MAX_LIGHTS;
		sData->LightInfo.SpotLightInfo[num_lights] = {light.Color, position, direction, light.Radius, glm::cos(glm::radians(light.InnerCutOff)), glm::cos(glm::radians(light.OuterCutOff))};

		ShadowRenderer::SubmitSpotLight(direction, position, light.Radius);
	}

	glm::uvec3 Renderer::GetNumLights()
	{
		auto &light_info = sData->LightInfo;

		return {light_info.NumDirectionalLights, light_info.NumPointLights, light_info.NumSpotLights};
	}

	void Renderer::End()
	{

		LineRenderer::End();
		QuadRenderer::End();
	}

	Ref<Texture> Renderer::GetWhiteTexture()
	{
		return sData->WhiteTexture;
	}

	Ref<Texture> Renderer::GetBlackTexture()
	{
		return sData->BlackTexture;
	}

	void Renderer::ResetStats()
	{
		memset(&sStats, 0, sizeof(Statitics));
	}

	const Frustum &Renderer::GetFrustum()
	{
		return CameraBuffer::Get().GetCameraData().ViewFrustum;
	}

	Renderer::RenderData *Renderer::sData = nullptr;

	CameraBuffer::CameraBuffer()
	{
		mBuffer = CreateRef<UniformBuffer>(CAMERA_UBO_BINDING, sizeof(FCameraData));
	}

	void CameraBuffer::Submit(const glm::mat4 &proj, const glm::mat4 &view)
	{
		mData.Projection = proj;
		mData.View = view;
		mData.NearFar.x = proj[3][2] / (proj[2][2] - 1.0f);
		mData.NearFar.y = proj[3][2] / (proj[2][2] + 1.0f);
		mData.CameraPosition = glm::inverse(view)[3];

		mBuffer->SetData(&mData, sizeof(FCameraData));

		mData.ViewFrustum.Update(proj, view);
	}

	CameraBuffer &CameraBuffer::Get()
	{
		static CameraBuffer buffer;
		return buffer;
	}
} // namespace BHive