#include "Renderer.h"
#include "gfx/textures/Texture2D.h"
#include "buffers/ModelBuffer.h"

namespace BHive
{

	struct Renderer::RenderData
	{
		CameraBuffer Camera;
		ModelBuffer Model;

		Ref<Texture> WhiteTexture;
		Ref<Texture> BlackTexture;
		Ref<Texture> BlueTexture;

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

			Camera.Init();
			Model.Init();
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
		sData->Camera.Begin(projection, view);
	}

	void Renderer::SubmitMesh(const Ref<FMeshRenderData> &data)
	{
		sData->Model.Submit(data);
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
		return sData->Camera.GetViewFrustum();
	}

	CameraBuffer &Renderer::GetCamera()
	{
		return sData->Camera;
	}

	Renderer::RenderData *Renderer::sData = nullptr;

} // namespace BHive