#include "Application.h"
#include "WindowInput.h"
#include "Time.h"
#include "gfx/RenderCommand.h"
#include "renderers/Renderer.h"
#include "audio/AudioContext.h"
#include "layers/ImGuiLayer.h"
#include "threading/Threading.h"
#include "FPSCounter.h"

namespace BHive
{

	Application::Application(const FApplicationSpecification &specification)
		: mSpecification(specification)
	{
		ASSERT(!sInstance);
		sInstance = this;

		if (!mSpecification.WorkingDirectory.empty())
			std::filesystem::current_path(mSpecification.WorkingDirectory);

		FWindowProperties props{};
		props.Title = specification.Title;
		props.Size = specification.Size;
		props.VSync = specification.VSync;
		props.mCenterWindow = specification.CenterWindow;
		props.Maximize = specification.Maximize;
		mWindow = CreateScope<Window>(props);

		FOnWindowInputEvent window_callback;
		window_callback.bind(this, &Application::OnEvent);
		mWindow->SetEventCallback(window_callback);

		RenderCommand::Init();
		Renderer::Init();

		mImGuiLayer = new ImGuiLayer(mWindow->GetNative());
		PushLayer(mImGuiLayer);
	}

	Application::~Application()
	{
		Renderer::Shutdown();

		sInstance = nullptr;
	}

	void Application::Run()
	{
		while (mIsRunning)
		{
			FPSCounter::Get().Frame();

			Window::PollEvents();

			UpdateLayersAndWindow();
		}
	}

	void Application::Close()
	{
		mIsRunning = false;
	}

	void Application::PushLayer(Layer *layer)
	{
		mLayerStack.Push(layer);
	}

	void Application::PopLayer(Layer *layer)
	{
		mLayerStack.Pop(layer);
	}

	void Application::OnEvent(Event &event)
	{

		EventDispatcher dispatcher(event);
		dispatcher.Dispatch(this, &Application::OnWindowClosed);
		dispatcher.Dispatch(this, &Application::OnWindowResized);

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

		static float lasttime = 0;
		float time = Time::Get();
		float deltatime = time - lasttime;
		lasttime = time;

		for (auto &layer : mLayerStack)
		{
			layer->OnUpdate(deltatime);
		}

		Thread::Update();

		mImGuiLayer->BeginFrame();

		for (auto &layer : mLayerStack)
		{
			layer->OnGuiRender();
		}

		mImGuiLayer->EndFrame();

		mWindow->Update();
	}

	bool Application::OnWindowResized(WindowResizeEvent &event)
	{
		UpdateLayersAndWindow();

		return false;
	}

	bool Application::OnWindowClosed(WindowCloseEvent &event)
	{
		Close();

		return false;
	}
} // namespace BHive