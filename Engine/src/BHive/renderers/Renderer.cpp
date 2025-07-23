#include "gfx/RenderCommand.h"
#include "gfx/Shader.h"
#include "gfx/textures/Texture2D.h"
#include "gfx/UniformBuffer.h"
#include "Renderer.h"
#include "ShadowRenderer.h"

#include <glad/glad.h>

#define CAMERA_UBO_BINDING 0
#define LIGHT_UBO_BINDING 1

namespace BHive
{
	struct LightData
	{
		alignas(16) glm::vec3 Position{0.f};
		alignas(16) glm::vec3 Direction{0.f};
		alignas(16) glm::vec3 Color{1.f};
		float Brightness{1.f};
		float Radius{1.f};
		float Cutoff{12.5f};
		float OuterCutOff{20.0f};
		uint32_t Type = 0;
	};

	struct Renderer::RenderData
	{

		struct FLightData
		{
			uint32_t mNumLights = 0;
			LightData mLights[MAX_LIGHTS] = {};
		};

		Ref<UniformBuffer> mObjectBuffer;
		Ref<UniformBuffer> mLightBuffer;

		Ref<Texture> mWhiteTexture;
		Ref<Texture> mBlackTexture;
		FLightData mLightData{};

		RenderData()
		{
			mLightBuffer = CreateRef<UniformBuffer>(LIGHT_UBO_BINDING, sizeof(FLightData));

			uint32_t white = 0xFFFFFFFF;
			FTextureSpecification texture_specs{};
			texture_specs.Channels = 3;
			texture_specs.InternalFormat = EFormat::RGB8;

			mWhiteTexture = CreateRef<Texture2D>(1, 1, texture_specs, &white, sizeof(uint32_t));

			uint32_t black = 0xFF000000;
			mBlackTexture = CreateRef<Texture2D>(1, 1, texture_specs, &black, sizeof(uint32_t));
		}
	};

	void Renderer::Init()
	{
		sData = new RenderData();

		ShadowRenderer::Init(MAX_LIGHTS);
		LineRenderer::Init();
		QuadRenderer::Init();
		MeshRenderer::Init();
	}

	void Renderer::Shutdown()
	{

		LineRenderer::Shutdown();
		QuadRenderer::Shutdown();
		MeshRenderer::Shutdown();

		delete sData;
	}

	void Renderer::Begin()
	{
		ResetStats();

		sData->mLightData.mNumLights = 0;
		sData->mLightBuffer->SetData(&sData->mLightData.mNumLights, sizeof(uint32_t));

		LineRenderer::Begin();
		QuadRenderer::Begin();
		MeshRenderer::Begin();
	}

	void Renderer::SubmitCamera(const glm::mat4 &projection, const glm::mat4 &view)
	{
		CameraBuffer::Get().Submit(projection, view);
	}

	void Renderer::SubmitDirectionalLight(const glm::vec3 &direction, const DirectionalLight &light)
	{
		SubmitLight(direction, {}, light.mColor, light.mBrightness, 0.0f, 0.0f, 0.0f, ELightType::Directional);
	}

	void Renderer::SubmitPointLight(const glm::vec3 &position, const PointLight &light)
	{

		SubmitLight({}, position, light.mColor, light.mBrightness, light.mRadius, 0.0f, 0.0f, ELightType::Point);
	}

	void Renderer::SubmitSpotLight(const glm::vec3 &direction, const glm::vec3 &position, const SpotLight &light)
	{
		SubmitLight(
			direction, position, light.mColor, light.mBrightness, light.mRadius, glm::cos(glm::radians(light.mInnerCutOff)),
			glm::cos(glm::radians(light.mOuterCutOff)), ELightType::SpotLight);
	}

	void Renderer::SubmitLight(
		const glm::vec3 &direction, const glm::vec3 &position, const FColor &color, float brightness, float radius,
		float cutoff, float outercutoff, ELightType type)
	{
		auto current_index = sData->mLightData.mNumLights % MAX_LIGHTS;
		sData->mLightData.mLights[current_index] = LightData{
			.Position = position,
			.Direction = direction,
			.Color = color,
			.Brightness = brightness,
			.Radius = radius,
			.Cutoff = cutoff,
			.OuterCutOff = outercutoff,
			.Type = (uint32_t)type};

		sData->mLightData.mNumLights++;
		sData->mLightBuffer->SetData(&sData->mLightData, sizeof(RenderData::FLightData));
	}

	void Renderer::End()
	{
		LineRenderer::End();
		QuadRenderer::End();
		MeshRenderer::End();
	}

	Ref<Texture> Renderer::GetWhiteTexture()
	{
		return sData->mWhiteTexture;
	}

	Ref<Texture> Renderer::GetBlackTexture()
	{
		return sData->mBlackTexture;
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