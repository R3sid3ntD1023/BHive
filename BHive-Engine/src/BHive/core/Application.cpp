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
#include "input/InputManager.h"
#include "gui/GUI.h"

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
		mMainWindow = mWindowManager.Create(props);

		WindowInput::WindowEvent.Add(this, &Application::OnEvent);

		auto api = RendererAPI::Create();
		mRenderer = CreateScope<Renderer>(std::move(api));

		if (specification.Flags & EApplicationFlags::EnableImGui)
		{
			auto layer = ImGuiLayer::Create(mMainWindow->GetNative());
			PushLayer(layer);
			mImGuiLayer = layer.get();
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
		LOG_TRACE("App Destructor Called");

		RenderCommand::Shutdown();

		if (mSpecification.Flags & EApplicationFlags::EnableAudio)
		{
			GetSubSystem<AudioContext>().Shutdown();
		}

		if (mSpecification.Flags & EApplicationFlags::EnablePhysics)
		{
			GetSubSystem<PhysicsContext>().Shutdown();
		}

		ShaderManager::Clear();

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
	}

	bool Application::OnWindowClosed(WindowCloseEvent &event)
	{
		Close();

		return false;
	}

} // namespace BHive