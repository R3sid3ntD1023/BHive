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

	enum EApplicationFlags : uint32_t
	{
		None = 0,
		EnableImGui = BIT(0),
		EnableAudio = BIT(1),
		EnableRendering = BIT(2),
		EnablePhysics = BIT(3),
		EnableAll = EnableImGui | EnableAudio | EnableRendering | EnablePhysics
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
		uint32_t Flags = EApplicationFlags::EnableAll;
	};

	class BHIVE_API Application
	{

	public:
		Application(const FApplicationSpecification &specification);

		virtual ~Application();

		void Run();

		void Close();

		template <typename TLayer>
		void PushLayer();

		void PushLayer(const Ref<Layer> &layer);

		void PopLayer(const Ref<Layer> &layer);

		virtual void OnEvent(Event &event);

	public:
		const FApplicationSpecification &GetSpecification() const { return mSpecification; }

		Window &GetWindow() { return *mMainWindow; }

		ImGuiLayer* GetImGuiLayer() { return mImGuiLayer.get(); }

		static Application &Get() { return *sInstance; }

	private:
		void UpdateLayersAndWindow();

		bool OnWindowResized(WindowResizeEvent &event);

		bool OnWindowClosed(WindowCloseEvent &event);

	private:
		bool mIsRunning = true;

		bool mIsMinimized = false;

		Window* mMainWindow = nullptr;

		Ref<ImGuiLayer> mImGuiLayer = nullptr;

		LayerStack mLayerStack;

		FApplicationSpecification mSpecification;

		Scope<Renderer> mRenderer;

		static inline Application *sInstance;
	};

	BHIVE_API Application *CreateApplication(const FCommandLine &commandline);

	template <typename TLayer>
	inline void Application::PushLayer()
	{
		PushLayer(CreateRef<TLayer>());
	}
} // namespace BHive