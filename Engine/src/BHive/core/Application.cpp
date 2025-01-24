#include "Application.h"
#include "WindowInput.h"
#include "Time.h"
#include "gfx/RenderCommand.h"
#include "renderers/Renderer.h"
#include "audio/AudioContext.h"
#include "gui/Gui.h"
#include "threading/Threading.h"

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
		props.Width = specification.Width;
		props.Height = specification.Height;
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

			while (mEventQueue.size())
			{
				mEventQueue.front()();
				mEventQueue.pop();
			}

			UpdateLayersAndWindow();

			Window::PollEvents();
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

	void Application::QueueEvent(const std::function<void()> &event)
	{
		mEventQueue.push(event);
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
			layer->OnGuiRender(deltatime);
		}

		mImGuiLayer->EndFrame(mWindow->GetWidth(), mWindow->GetHeight());

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