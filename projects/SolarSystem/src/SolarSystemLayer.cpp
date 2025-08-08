#include "SolarSystemLayer.h"
#include <core/Application.h>
#include <core/FPSCounter.h>

#include "core/profiler/ProfilerViewer.h"

#include <core/FileDialog.h>
#include <gfx/RenderCommand.h>
#include <gfx/Shader.h>
#include <implot.h>

#include "gfx/ShaderManager.h"
#include "utils/ImageUtils.h"
#include <font/Font.h>
#include <font/FontManager.h>

#include "renderers/SceneRenderer.h"
#include "world/World.h"
#include "ResourceManager.h"
#include "gfx/textures/Texture2D.h"

BEGIN_NAMESPACE(BHive)

UUID mSelectedID{"f68f4384-d55f-4a9b-872e-efe27fa06659"};

void SolarSystemLayer::OnAttach()
{
	auto &window = Application::Get().GetWindow();
	auto window_size = window.GetSize();

	auto font = FontManager::Get().AddFontFromFile(ENGINE_PATH "/data/fonts/Roboto/Roboto-Regular.ttf", 96);

	mResourceManager = CreateRef<ResourceManager>(RESOURCE_PATH);
	AssetManager::SetAssetManager(&*mResourceManager);
	mRenderer = CreateRef<SceneRenderer>();
	mRenderer->Initialize(window_size.x, window_size.y);

	{
		try
		{
			std::ifstream in(RESOURCE_PATH "/Scenes/Universe.json");
			cereal::JSONInputArchive ar(in);

			mWorld = CreateRef<World>();
			ar(*mWorld);
		}
		catch (std::exception &e)
		{
			LOG_ERROR("Failed to load world- Error:{}", e.what());
			return;
		}
	}

	mCamera = EditorCamera(45.f, window_size.x / (float)window_size.y, .01f, 1000.00f);

	RenderCommand::ClearColor(.1f, .1f, .1f, 1.f);
	RenderCommand::SetLineWidth(1.0f);

	mWorld->Begin();
}

void SolarSystemLayer::OnDetach()
{
	if (mWorld)
		mWorld->End();
}

void SolarSystemLayer::OnUpdate(float dt)
{
	mCamera.ProcessInput();

	mRenderer->Begin(&mCamera, mCamera.GetView());

	LineRenderer::DrawGrid(FGrid{});

	if (mWorld)
		mWorld->Update(dt);

	mRenderer->End();

	mRenderer->RenderToScreen();
}

void SolarSystemLayer::OnEvent(Event &e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch(this, &SolarSystemLayer::OnWindowResize);
	mCamera.OnEvent(e);
}

void SolarSystemLayer::OnGuiRender()
{
	if (ImGui::Begin("Performance"))
	{
		ProfilerViewer::ViewFPS();
		ProfilerViewer::ViewCPUGPU();
	}

	ImGui::End();

	if (ImGui::Begin("Depth"))
	{
		auto size = ImGui::GetContentRegionAvail();
		auto texture = mRenderer->GetDepthAttachment();
		ImGui::Image((ImTextureID)(uint64_t)(uint32_t)*texture, size, {0, 1}, {1, 0});
	}
	ImGui::End();

	if (ImGui::Begin("Save"))
	{
		if (ImGui::Button("Save"))
		{
			auto path = FileDialogs::SaveFile("PNG (.png)\0*.png\0");
			if (!path.empty())
			{
				ImageUtils::SaveImage(path, Cast<Texture2D>(mRenderer->GetColorAttachment()));
			}
		}
	}

	ImGui::End();
}

bool SolarSystemLayer::OnWindowResize(WindowResizeEvent &e)
{
	mCamera.Resize(e.x, e.y);
	mRenderer->Resize(e.x, e.y);

	return false;
}

END_NAMESPACE
