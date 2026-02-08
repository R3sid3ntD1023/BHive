#include "buffers/GlobalBuffers.h"
#include "buffers/ModelBuffer.h"
#include "gfx/Texture.h"
#include "Renderer.h"

namespace BHive
{

	struct Renderer::RenderData
	{
		ModelBuffer Model;

		Ref<Texture> WhiteTexture;
		Ref<Texture> BlackTexture;
		Ref<Texture> BlueTexture;

		RenderData()
		{
			
			static constexpr uint32_t white = 0xFFFFFFFF;
			static constexpr uint32_t black = 0xFF000000;
			static constexpr uint32_t blue	= 0xFF0000FF;

			FTextureCreateInfo create_info{};
			create_info.Channels = 4;
			create_info.InternalFormat = EFormat::RGBA8;

			WhiteTexture = Texture2D::Create(1, 1, create_info, &white, sizeof(uint32_t));
			
			BlackTexture = Texture2D::Create(1, 1, create_info, &black, sizeof(uint32_t));
	
			BlueTexture = Texture2D::Create(1, 1, create_info, &blue, sizeof(uint32_t));

			GlobalBuffers::CameraData.Init();
			// Model.Init();
		}
	};

	void Renderer::Init()
	{
		sData = new RenderData();

		LineRenderer::Init();
		// QuadRenderer::Init();
	}

	void Renderer::Shutdown()
	{

		LineRenderer::Shutdown();
		// QuadRenderer::Shutdown();

		delete sData;
	}

	void Renderer::Begin()
	{
		ResetStats();

		LineRenderer::Begin();
		// QuadRenderer::Begin();
	}

	void Renderer::SubmitCamera(const glm::mat4 &projection, const glm::mat4 &view)
	{
		GlobalBuffers::CameraData.Begin(projection, glm::inverse(view));
	}

	void Renderer::Draw(const Ref<FMeshRenderData> &data)
	{
		// sData->Model.Draw(data);
	}

	void Renderer::End()
	{

		LineRenderer::End();
		// QuadRenderer::End();
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
		return GlobalBuffers::CameraData.GetViewFrustum();
	}

	CameraBuffer &Renderer::GetCamera()
	{
		return GlobalBuffers::CameraData;
	}

	Renderer::RenderData *Renderer::sData = nullptr;

} // namespace BHive