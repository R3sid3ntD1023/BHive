#include "Renderer.h"
#include "gfx/RenderCommand.h"
#include "gfx/Shader.h"
#include "gfx/UniformBuffer.h"
#include "gfx/Texture.h"
#include "ShadowRenderer.h"
#include <glad/glad.h>

#define MAX_BONES 128

namespace BHive
{
	struct LightData
	{
		alignas(16) glm::vec3 Position{ 0.f };
		alignas(16) glm::vec3 Direction{ 0.f };
		alignas(16) glm::vec3 Color{ 1.f };
		float Brightness{ 1.f };
		float Radius{ 1.f };
		float Cutoff{ 12.5f };
		float OuterCutOff{ 20.0f };
		uint32_t Type = 0;
	};

	struct Renderer::RenderData
	{
		struct FObjectData
		{
			glm::mat4 projection{ 1.0f };
			glm::mat4 view{1.0f};
			glm::mat4 model{ 1.0f };
		};

		struct FLightData
		{
			uint32_t mNumLights = 0;
			//uint32_t mNumPointLights = 0;
			LightData mLights[MAX_LIGHTS] = {};
			
		};

		Ref<UniformBuffer> mObjectBuffer;
		Ref<UniformBuffer> mLightBuffer;
		Ref<UniformBuffer> mBoneBuffer;

		Ref<Texture> mWhiteTexture;
		Ref<Texture> mBlackTexture;

		FObjectData mObjectData{};
		FLightData mLightData{};

		RenderData()
		{
			mObjectBuffer = UniformBuffer::Create(0, sizeof(FObjectData));
			mLightBuffer = UniformBuffer::Create(1, sizeof(FLightData));
			mBoneBuffer = UniformBuffer::Create(5, sizeof(glm::mat4) * MAX_BONES);

			uint32_t white = 0xFFFFFFFF;
			FTextureSpecification texture_specs{};
			texture_specs.Channels = 3;
			texture_specs.Format = EFormat::RGB8;

			mWhiteTexture = Texture2D::Create(&white, 1, 1, texture_specs);

			uint32_t black = 0xFF000000;
			mBlackTexture = Texture2D::Create(&black, 1, 1, texture_specs);
		}
	};

	void Renderer::Init()
	{
		utils::SetIncludeFromFile("/Core.glsl", ENGINE_PATH"/data/shaders/Core.glsl");
		utils::SetIncludeFromFile("/Lighting.glsl", ENGINE_PATH"/data/shaders/Lighting.glsl");
		utils::SetIncludeFromFile("/Skinning.glsl", ENGINE_PATH"/data/shaders/Skinning.glsl");
		utils::SetIncludeFromFile("/BDRFFunctions.glsl", ENGINE_PATH"/data/shaders/BDRFFunctions.glsl");

		sData = new RenderData();

		ShadowRenderer::Init(MAX_LIGHTS);
		LineRenderer::Init();
		BillboardRenderer::Init();
		QuadRenderer::Init();

		
	}

	void Renderer::Shutdown()
	{
		
		LineRenderer::Shutdown();
		BillboardRenderer::Shutdown();
		QuadRenderer::Shutdown();

		delete sData;
	}

	void Renderer::Begin(const glm::mat4& projection, const glm::mat4& view)
	{
		ResetStats();

		sData->mObjectData.projection = projection;
		sData->mObjectData.view = view;
		sData->mObjectBuffer->SetData(sData->mObjectData);
		sData->mLightData.mNumLights = 0;
		//sData->mLightData.mNumPointLights = 0;
		sData->mLightBuffer->SetData(sData->mLightData.mNumLights);
		//sData->mLightBuffer->SetData(sData->mLightData.mNumPointLights, sizeof(uint32_t));

		LineRenderer::Begin();
		BillboardRenderer::Begin();
		QuadRenderer::Begin();
	}

	void Renderer::SubmitDirectionalLight(const glm::vec3& direction, const DirectionalLight& light)
	{
		SubmitLight(direction, {}, light.mColor, light.mBrightness, 0.0f, 0.0f, 0.0f, ELightType::Directional);
	}

	void Renderer::SubmitPointLight(const glm::vec3& position, const PointLight& light)
	{

		SubmitLight({}, position, light.mColor, light.mBrightness, light.mRadius, 0.0f, 0.0f, ELightType::Point);
	}

	void Renderer::SubmitSpotLight(const glm::vec3& direction, const glm::vec3& position, const SpotLight& light)
	{
		SubmitLight(direction, position, light.mColor, light.mBrightness, light.mRadius, glm::cos(glm::radians(light.mInnerCutOff)), 
			glm::cos(glm::radians(light.mOuterCutOff)), ELightType::SpotLight);
	}

	void Renderer::SubmitLight(const glm::vec3& direction, const glm::vec3& position, const Color& color, float brightness, float radius, float cutoff, float outercutoff, 
		ELightType type)
	{
		auto current_index = sData->mLightData.mNumLights % MAX_LIGHTS;
		sData->mLightData.mLights[current_index] = LightData{ .Position = position, .Direction = direction,
			.Color = color, .Brightness = brightness, .Radius = radius, .Cutoff = cutoff, .OuterCutOff = outercutoff, .Type = (uint32_t)type};

		sData->mLightData.mNumLights++;
		sData->mLightBuffer->SetData(sData->mLightData);
	}

	void Renderer::SubmitTransform(const glm::mat4& transform)
	{
		sData->mObjectData.model = transform;
		sData->mObjectBuffer->SetData(sData->mObjectData);
	}

	void Renderer::SubmitSkeletalMesh(const std::vector<glm::mat4>& bone_matrices)
	{
		sData->mBoneBuffer->SetData(bone_matrices);
	}

	void Renderer::End()
	{
		LineRenderer::End();
		BillboardRenderer::End();
		QuadRenderer::End();
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

	const glm::mat4& Renderer::GetView()
	{
		return sData->mObjectData.view;
	}

	Renderer::RenderData* Renderer::sData = nullptr;

	Renderer::Statitics Renderer::sStats;
}