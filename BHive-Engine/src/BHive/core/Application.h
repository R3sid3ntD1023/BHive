#pragma once

#include "core/Core.h"
#include "core/Layer.h"
#include "core/Window.h"
#include "events/ApplicationEvents.h"
#include "gfx/renderers/Renderer.h"

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

		virtual ~Application();

		void Run();

		void Close();

		void BlockImGuiEvents(bool block);

		template <typename TLayer>
		void PushLayer();

		void PushLayer(const Ref<Layer> &layer);

		void PopLayer(const Ref<Layer> &layer);

		ImGuiLayer *GetImGuiLayer() const { return mImGuiLayer; }

		virtual void OnEvent(Event &event);

	public:
		const FApplicationSpecification &GetSpecification() const { return mSpecification; }

		Window &GetWindow() { return *mMainWindow; }

		static Application &Get() { return *sInstance; }

	private:
		void UpdateLayersAndWindow();

		bool OnWindowClosed(WindowCloseEvent &event);

	private:
		bool mIsRunning = true;

		Scope<Renderer> mRenderer;

		WindowManager mWindowManager;

		LayerStack mLayerStack;

		Window *mMainWindow = nullptr;

		ImGuiLayer *mImGuiLayer = nullptr;

		FApplicationSpecification mSpecification;

		static inline Application *sInstance = nullptr;
	};

	BHIVE_API Application *CreateApplication(const FCommandLine &commandline);

	template <typename TLayer>
	inline void Application::PushLayer()
	{
		PushLayer(CreateRef<TLayer>());
	}
} // namespace BHive