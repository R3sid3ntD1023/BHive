#include "buffers/ModelBuffer.h"
#include "buffers/LightBuffer.h"
#include "gfx/Texture.h"
#include "Renderer.h"
#include "gfx/Buffers.h"
#include "gfx/GlobalBuffers.h"
#include "PMREMGenerator.h"

namespace BHive
{
	struct BHIVE_API FObjectData
	{
		glm::mat4 WorldMatrix = {1.0f};
	};

	struct BHIVE_API Renderer::RenderData
	{
		FCameraData CameraData;
		Frustum CameraFrustum;
		FModelBuffer ModelBuffer;
		LightBuffer LightingBuffer;

		Ref<Texture> WhiteTexture;
		Ref<Texture> BlackTexture;
		Ref<Texture> BlueTexture;

		Ref<GPUBuffer> CameraUniformBuffer;

		PMREMGenerator EnvironmentMapGenerator;

		RenderData()
		{
			auto& global = AddSubSystem<GlobalBuffers>();

			static constexpr uint32_t white = 0xFFFFFFFF;
			static constexpr uint32_t black = 0xFF000000;
			static constexpr uint32_t blue = 0xFF0000FF;

			FTextureCreateInfo create_info{};
			create_info.Format = EFormat::RGBA8;
			create_info.Roles = ETextureRole::Sampled | ETextureRole::TransferDst;
			create_info.Aspect = ETextureAspect::Color;
			create_info.DebugName = "WhiteTexture";

			WhiteTexture = Texture2D::Create({1, 1}, create_info, Buffer(& white, sizeof(uint32_t)));

			create_info.DebugName = "Black Texture";
			BlackTexture = Texture2D::Create({1, 1}, create_info, Buffer(&black, sizeof(uint32_t)));

			create_info.DebugName = "Blue Texture";
			BlueTexture = Texture2D::Create({1, 1}, create_info, Buffer(&blue, sizeof(uint32_t)));

			CameraUniformBuffer = GPUBuffer::Create(sizeof(FCameraData), EBufferType::UniformBuffer);

			global.Register(0, CameraUniformBuffer);

			ModelBuffer.Init();
			LightingBuffer.Init();

			EnvironmentMapGenerator.Initialize();
		}

		~RenderData() { RemoveSubSystem<GlobalBuffers>();
		}
	};

	void Renderer::Init()
	{
		sData = new RenderData();

		LineRenderer::Init();
		QuadRenderer::Init();
	}

	void Renderer::Shutdown()
	{

		LineRenderer::Shutdown();
		QuadRenderer::Shutdown();

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
		sData->CameraData.Projection = projection;
		sData->CameraData.View = view;
		sData->CameraData.NearFar.x = projection[3][2] / (projection[2][2] - 1.0f);
		sData->CameraData.NearFar.y = projection[3][2] / (projection[2][2] + 1.0f);
		sData->CameraData.Position = glm::inverse(view)[3];

		sData->CameraUniformBuffer->SetData(&sData->CameraData, sizeof(FCameraData));

		sData->CameraFrustum.Update(projection, view);

	}

	void Renderer::End()
	{

		LineRenderer::End();
		QuadRenderer::End();
	}


	void Renderer::SetEnvironmentTexture(const Ref<Texture> &texture)
	{
		sData->EnvironmentMapGenerator.SetEnvironmentMap(texture);

		auto &irradiance = sData->EnvironmentMapGenerator.GetIrradianceTexture();
		auto &bdrflut = sData->EnvironmentMapGenerator.GetBDRFLUT();
		auto &prefilter = sData->EnvironmentMapGenerator.GetPreFilteredEnvironmentTexture();

		auto &global_buffer = GetSubSystem<GlobalBuffers>();
		global_buffer.Register(0, irradiance);
		global_buffer.Register(1, bdrflut);
		global_buffer.Register(2, prefilter);
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

	FModelBuffer &Renderer::GetModelBuffer()
	{
		return sData->ModelBuffer;
	}

	Ref<Texture> Renderer::GetPreFilterEnvironmentTexture()
	{
		return sData->EnvironmentMapGenerator.GetPreFilteredEnvironmentTexture();
	}

	Ref<Texture> Renderer::GetEnviromentCubeTexture()
	{
		return sData->EnvironmentMapGenerator.GetEnvironmentCubeTexture();
	}

	Ref<Texture> Renderer::GetIrradianceTexture()
	{
		return sData->EnvironmentMapGenerator.GetIrradianceTexture();
	}

	Ref<Texture> Renderer::GetBRDFLUTTexture()
	{
		return sData->EnvironmentMapGenerator.GetBDRFLUT();
	}

	const Frustum &Renderer::GetFrustum()
	{
		ASSERT(sData);
		return sData->CameraFrustum;
	}

	FCameraData &Renderer::GetCameraData()
	{
		ASSERT(sData);
		return sData->CameraData;
	}

	Renderer::RenderData *Renderer::sData = nullptr;

} // namespace BHive