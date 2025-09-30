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

		if (specification.Flags & EApplicationFlags::EnableRendering)
		{
			RenderCommand::Init();
			Renderer::Init();
		}

		if (specification.Flags & EApplicationFlags::EnableImGui)
		{
			mImGuiLayer = new ImGuiLayer(mWindow->GetNative());
			PushLayer(mImGuiLayer);
		}

		if (specification.Flags & EApplicationFlags::EnableAudio)
		{
			GetSubSystem<AudioContext>().Init();
		}

		if (mSpecification.Flags & EApplicationFlags::EnablePhysics)
		{
			AddSubSystem<PhysicsContext>().Init();
		}

		AddSubSystem<UndoRedo>();
	}

	Application::~Application()
	{
		if (mSpecification.Flags & EApplicationFlags::EnableRendering)
		{
			Renderer::Shutdown();

			sInstance = nullptr;
		}

		if (mSpecification.Flags & EApplicationFlags::EnableAudio)
		{
			GetSubSystem<AudioContext>().Shutdown();
		}

		if (mSpecification.Flags & EApplicationFlags::EnablePhysics)
		{
			GetSubSystem<PhysicsContext>().Shutdown();
		}
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

		if (mImGuiLayer)
		{
			mImGuiLayer->BeginFrame();

			for (auto &layer : mLayerStack)
			{
				layer->OnGuiRender();
			}

			mImGuiLayer->EndFrame();
		}

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