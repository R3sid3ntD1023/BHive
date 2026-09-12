#include "Application.h"
#include "FPSCounter.h"
#include "Time.h"
#include "WindowInput.h"
#include "audio/AudioContext.h"
#include "gfx/RenderCommand.h"
#include "gfx/ShaderManager.h"
#include "gfx/renderers/Renderer.h"
#include "gui/GUI.h"
#include "input/InputManager.h"
#include "layers/ImGuiLayer.h"
#include "physics/PhysicsContext.h"
#include "subsystem/SubSystem.h"
#include "threading/Threading.h"
#include "undoredo/UndoRedo.h"

namespace BHive
{

	Application::Application(const FApplicationSpecification &specification)
		: mSpecification(specification)
	{

		ASSERT(!sInstance);
		sInstance = this;

		if (!mSpecification.WorkingDirectory.empty())
			std::filesystem::current_path(mSpecification.WorkingDirectory);

		RenderCommand::Init(RendererAPI::Vulkan);

		Window::Init();

		auto api = RendererAPI::Create();
		mRenderer = CreateScope<Renderer>(std::move(api));

		FWindowProperties props{};
		props.Title = specification.Title;
		props.Size = specification.Size;
		props.VSync = specification.VSync;
		props.mCenterWindow = specification.CenterWindow;
		props.Maximize = specification.Maximize;
		mMainWindow = mWindowManager.Create(props);

		WindowInput::WindowEvent.Add(this, &Application::OnEvent);

		auto layer = ImGuiLayer::Create(mMainWindow->GetNative());
		PushLayer(layer);
		mImGuiLayer = layer.get();

		if (EngineConfig::EnableAudio)
		{
			GetSubSystem<AudioContext>().Init();
		}

		if (EngineConfig::EnablePhysics)
		{
			AddSubSystem<PhysicsContext>().Init();
		}

		AddSubSystem<UndoRedo>();
	}

	Application::~Application()
	{
		LOG_TRACE("App Destructor Called");

		RenderCommand::Shutdown();

		if (EngineConfig::EnableAudio)
		{
			GetSubSystem<AudioContext>().Shutdown();
		}

		if (EngineConfig::EnablePhysics)
		{
			GetSubSystem<PhysicsContext>().Shutdown();
		}

		sInstance = nullptr;
	}

	void Application::Run()
	{
		while (mIsRunning)
		{
			auto &input = InputManager::Get();

			input.BeginFrame();

			Window::PollEvents();

			if (!mMainWindow->IsMinimized())
			{
				FPSCounter::Get().Frame();

				UpdateLayersAndWindow();
			}

			input.EndFrame();
		}
	}

	void Application::Close()
	{
		mIsRunning = false;
	}

	void Application::BlockImGuiEvents(bool block)
	{
		if (mImGuiLayer)
			mImGuiLayer->BlockEvents(block);
	}

	void Application::PushLayer(const Ref<Layer> &layer)
	{
		mLayerStack.Push(layer);
		layer->OnAttach(*this);
	}

	void Application::PopLayer(const Ref<Layer> &layer)
	{
		layer->OnDetach();
		mLayerStack.Pop(layer);
	}

	void Application::OnEvent(Event &event)
	{

		EventDispatcher dispatcher(event);
		dispatcher.Dispatch(this, &Application::OnWindowClosed);

		for (auto it = mLayerStack.rbegin(); it != mLayerStack.rend(); it++)
		{
			if (event)
			{
				break;
			}

			(*it)->OnEvent(event);
		}
	}

	void Application::UpdateLayersAndWindow()
	{
		const double targetFPS = EngineConfig::TargetFPS;
		const double targetFrameTime = 1.0 / targetFPS;
		auto frameStart = std::chrono::high_resolution_clock::now();

		Time::Update();
		auto dt = Time::DeltaTime();

		for (auto &layer : mLayerStack)
		{
			layer->OnUpdate(dt);
		}

		mRenderer->BeginFrame();

		for (auto &layer : mLayerStack)
			layer->OnRender(*mRenderer);

		if (mImGuiLayer)
		{
			mImGuiLayer->BeginFrame();

			GUI::BeginDockSpace("Dockspace");

			for (auto &layer : mLayerStack)
			{
				layer->OnGuiRender();
			}

			GUI::EndDockSpace();

			mImGuiLayer->EndFrame();
		}

		mRenderer->EndFrame();

		mWindowManager.Update(dt);

		Thread::Update();

		auto frameEnd = std::chrono::high_resolution_clock::now();
		double elapsed = std::chrono::duration<double>(frameEnd - frameStart).count();
		if (elapsed < targetFrameTime)
		{
			std::this_thread::sleep_for(std::chrono::duration<double>(targetFrameTime - elapsed));
		}
	}

	bool Application::OnWindowClosed(WindowCloseEvent &event)
	{
		Close();

		return false;
	}

} // namespace BHive