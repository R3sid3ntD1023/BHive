#include "buffers/ModelBuffer.h"
#include "gfx/Texture.h"
#include "Renderer.h"
#include "gfx/UniformBuffer.h"
#include "buffers/GlobalBuffers.h"
#include "gfx/StorageBuffer.h"

namespace BHive
{
	struct FObjectData
	{
		glm::mat4 WorldMatrix = {1.0f};
	};

	struct Renderer::RenderData
	{
		FCameraData CameraData;
		Frustum CameraFrustum;

		ModelBuffer Model;

		Ref<Texture> WhiteTexture;
		Ref<Texture> BlackTexture;
		Ref<Texture> BlueTexture;

		Ref<UniformBuffer> CameraUniformBuffer;

		RenderData()
		{

			static constexpr uint32_t white = 0xFFFFFFFF;
			static constexpr uint32_t black = 0xFF000000;
			static constexpr uint32_t blue = 0xFF0000FF;

			FTextureCreateInfo create_info{};
			create_info.Format = EFormat::RGBA8;
			create_info.Usage = ETextureUsage::Sampled | ETextureUsage::TransferDst;
			create_info.Aspect = ETextureAspect::Color;

			WhiteTexture = Texture2D::Create({1, 1}, create_info, Buffer(& white, sizeof(uint32_t)));

			BlackTexture = Texture2D::Create({1, 1}, create_info, Buffer(&black, sizeof(uint32_t)));

			BlueTexture = Texture2D::Create({1, 1}, create_info, Buffer(&blue, sizeof(uint32_t)));

			CameraUniformBuffer = UniformBuffer::Create(0, sizeof(FCameraData));
			GlobalBuffers::AddGlobalUniformBuffer(0, CameraUniformBuffer);

			Model.Init();
		}

		~RenderData() { CameraUniformBuffer.reset();}
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

	void Renderer::Draw(const Ref<FMeshRenderData> &data)
	{
		sData->Model.Draw(data);
	}

	void Renderer::SubmitModel(const FTransform &transform)
	{
		sData->Model.SubmitModel(transform);
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