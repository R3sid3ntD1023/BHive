#pragma once

#include "core/Core.h"
#include "core/Window.h"
#include "core/Layer.h"
#include "events/ApplicationEvents.h"

namespace BHive
{
	class ImGuiLayer;

	struct BHIVE_API FCommandLine
	{
		int Count = 0;
		char **Args = nullptr;

		const char *operator[](int index) const
		{
			ASSERT(index >= 0 && index < Count);
			return Args[index];
		}
	};

	struct BHIVE_API FApplicationSpecification
	{
		FCommandLine CommandLine;
		std::string Title = "BHive Application";
		std::string WorkingDirectory;
		glm::ivec2 Size{800, 600};
		bool VSync = true;

		// window
		bool CenterWindow = true;
		bool Maximize = false;
	};

	class BHIVE_API Application
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
		void SubmitToMainQueue(const std::function<void()> &func);

	public:
		const FApplicationSpecification &GetSpecification() const { return mSpecification; }
		Window &GetWindow() { return *mWindow; }
		ImGuiLayer &GetImGuiLayer() { return *mImGuiLayer; }
		static Application &Get() { return *sInstance; }

	protected:
		virtual void OnBeginGUIRender();
		virtual void OnEndGUIRender();

	private:
		void UpdateLayersAndWindow();
		bool OnWindowResized(WindowResizeEvent &event);
		bool OnWindowClosed(WindowCloseEvent &event);
		void ProcessMainQueue();

	private:
		bool mIsRunning = true;
		Scope<Window> mWindow;
		LayerStack mLayerStack;
		FApplicationSpecification mSpecification;
		ImGuiLayer *mImGuiLayer = nullptr;
		std::queue<std::function<void()>> mMainQueue;

		static inline Application *sInstance;
	};

	BHIVE_API Application *CreateApplication(const FCommandLine &commandline);

	template <typename TLayer>
	inline void Application::PushLayer()
	{
		PushLayer(new TLayer());
	}
} // namespace BHive