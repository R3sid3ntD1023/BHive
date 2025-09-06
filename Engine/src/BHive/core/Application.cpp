#include "Application.h"
#include "FPSCounter.h"
#include "gfx/RenderCommand.h"
#include "layers/ImGuiLayer.h"
#include "renderers/Renderer.h"
#include "threading/Threading.h"
#include "Time.h"
#include "WindowInput.h"
#include "subsystem/SubSystem.h"
#include "undoredo/UndoRedo.h"
#include "physics/PhysicsContext.h"
#include "audio/AudioContext.h"

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

		AddSubSystem<UndoRedo>();
		AddSubSystem<AudioContext>().Init();
		AddSubSystem<PhysicsContext>().Init();
	}

	Application::~Application()
	{
		Renderer::Shutdown();

		GetSubSystem<PhysicsContext>().Shutdown();
		GetSubSystem<AudioContext>().Shutdown();

		sInstance = nullptr;
	}

	void Application::Run()
	{
		while (mIsRunning)
		{

			FPSCounter::Get().Frame();

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

		mImGuiLayer->BeginFrame();

		for (auto &layer : mLayerStack)
		{
			layer->OnGuiRender();
		}

		mImGuiLayer->EndFrame();

		mWindow->Update();

		Thread::Update();
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