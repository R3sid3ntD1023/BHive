#include "gfx/RenderCommand.h"
#include "gfx/Shader.h"
#include "gfx/textures/Texture2D.h"
#include "gfx/UniformBuffer.h"
#include "gfx/StorageBuffer.h"
#include "Renderer.h"
#include "ShadowRenderer.h"

#include <glad/glad.h>

#define CAMERA_UBO_BINDING 0

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

		Ref<Texture> WhiteTexture;
		Ref<Texture> BlackTexture;
		Ref<Texture> BlueTexture;

		FLightInfo LightInfo{};

		RenderData()
		{
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

		LineRenderer::Begin();
		QuadRenderer::Begin();
	}

	void Renderer::SubmitCamera(const glm::mat4 &projection, const glm::mat4 &view)
	{
		CameraBuffer::Get().Submit(projection, view);
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