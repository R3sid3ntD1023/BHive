#include "Application.h"
#include "audio/AudioContext.h"
#include "FPSCounter.h"
#include "gfx/RenderCommand.h"
#include "layers/ImGuiLayer.h"
#include "physics/PhysicsContext.h"
#include "gfx/renderers/Renderer.h"
#include "subsystem/SubSystem.h"
#include "threading/Threading.h"
#include "Time.h"
#include "undoredo/UndoRedo.h"
#include "WindowInput.h"
#include "gfx/ShaderManager.h"

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

		FWindowProperties props{};
		props.Title = specification.Title;
		props.Size = specification.Size;
		props.VSync = specification.VSync;
		props.mCenterWindow = specification.CenterWindow;
		props.Maximize = specification.Maximize;
		mMainWindow = WindowManager::Get().Create(props);

		mMainWindow->GetWindowInput().WindowEvent.Add(this, &Application::OnEvent);

		if (specification.Flags & EApplicationFlags::EnableRendering)
		{			
			auto api = RendererAPI::Create();
			mRenderer = CreateScope<Renderer>(std::move(api));
		}

		if (specification.Flags & EApplicationFlags::EnableImGui)
		{
			mImGuiLayer = ImGuiLayer::Create(mMainWindow->GetNative());
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
			ShaderManager::Clear();
			WindowManager::Get().Shutdown();

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
		while (mIsRunning )
		{
			Window::PollEvents();

			if (!mIsMinimized)
			{
				FPSCounter::Get().Frame();
	
				UpdateLayersAndWindow();
			}
		}
	}

	void Application::Close()
	{
		mIsRunning = false;
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

		Time::Update();

		for (auto &layer : mLayerStack)
		{
			layer->OnUpdate(Time::DeltaTime());
		}

		mRenderer->BeginFrame();

		for (auto &layer : mLayerStack)
			layer->OnRender(*mRenderer);

		if (mImGuiLayer)
		{
			mImGuiLayer->BeginFrame();

			for (auto &layer : mLayerStack)
			{
				layer->OnGuiRender();
			}

			mImGuiLayer->EndFrame();
		}

		mRenderer->EndFrame();

		auto &window_manager = WindowManager::Get();
		for (auto &window : window_manager.GetWindows())
		{
			window->Update();
		}

		Thread::Update();
	}

	bool Application::OnWindowResized(WindowResizeEvent &event)
	{
		mIsMinimized = (event.x == 0 || event.y == 0);

		return false;
	}

	bool Application::OnWindowClosed(WindowCloseEvent &event)
	{
		Close();

		return false;
	}

} // namespace BHive