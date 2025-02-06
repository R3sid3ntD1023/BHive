#pragma once

#include "core/Core.h"
#include "core/Window.h"
#include "core/Layer.h"
#include "events/ApplicationEvents.h"

namespace BHive
{
	class ImGuiLayer;

	struct FCommandLine
	{
		int Count = 0;
		char **Args = nullptr;

		const char *operator[](int index) const
		{
			ASSERT(index >= 0 && index < Count);
			return Args[index];
		}
	};

	struct FApplicationSpecification
	{
		FCommandLine CommandLine;
		std::string Title = "BHive Application";
		std::string WorkingDirectory;
		uint32_t Width = 800, Height = 600;
		bool VSync = true;

		// window
		bool CenterWindow = true;
		bool Maximize = false;
	};

	class BHIVE Application
	{

	public:
		Application(const FApplicationSpecification &specification);
		~Application();

		void Run();
		void Close();

		template <typename TLayer>
		void PushLayer();

		void PushLayer(Layer *layer);
		void PopLayer(Layer *layer);

		virtual void OnEvent(Event &event);
		void QueueEvent(const std::function<void()> &event);

	public:
		const FApplicationSpecification &GetSpecification() const { return mSpecification; }
		Window &GetWindow() { return *mWindow; }
		ImGuiLayer &GetImGuiLayer() { return *mImGuiLayer; }
		static Application &Get() { return *sInstance; }

	private:
		void UpdateLayersAndWindow();
		bool OnWindowResized(WindowResizeEvent &event);
		bool OnWindowClosed(WindowCloseEvent &event);

	private:
		bool mIsRunning = true;
		Scope<Window> mWindow;
		LayerStack mLayerStack;
		FApplicationSpecification mSpecification;
		ImGuiLayer *mImGuiLayer = nullptr;

		std::queue<std::function<void()>> mEventQueue;
		static inline Application *sInstance = nullptr;
	};

	BHIVE Application *CreateApplication(const FCommandLine &commandline);

	template <typename TLayer>
	inline void Application::PushLayer()
	{
		PushLayer(new TLayer());
	}
} // namespace BHive