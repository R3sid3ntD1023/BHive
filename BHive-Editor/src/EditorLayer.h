#pragma once

#include "asset/EditorAssetManager.h"
#include "core/Command.h"
#include "core/events/ApplicationEvents.h"
#include "core/events/KeyEvents.h"
#include "core/Layer.h"
#include "EditorContentBrowser.h"
#include "gfx/cameras/EditorCamera.h"
#include "PropertiesPanel.h"
#include "rttr/library.h"
#include "SceneHeirarchyPanel.h"
#include "world/World.h"

namespace BHive
{
	class Framebuffer;

	class EditorLayer : public Layer
	{
	private:
		Ref<Framebuffer> mFramebuffer;
		Ref<World> mEditorWorld;
		Ref<World> mActiveWorld;

		Ref<SceneHierarchyPanel> mSceneHeirarchyPanel;
		Ref<PropertiesPanel> mPropertiesPanel;
		Ref<EditorContentBrowser<EditorAssetManager>> mContentBrowser;
		EditorCamera mEditorCamera;

		Ref<EditorAssetManager> mAssetManager;

		glm::ivec2 mViewportSize{};
		glm::ivec2 mViewportBounds[2] = {};
		bool mViewportHovered{false}, mViewportFocused{false};

		int32_t mGizmoOperation = 0;
		int32_t mGizmoMode = 0;
		bool mSnappingEnabled{false};
		float mMenuBarHeight = 10.f;

		std::unordered_map<FCommand, std::function<void()>> mCommands;

		std::filesystem::path mCurrentWorldPath = "";

		rttr::library *mProjectLib = nullptr;

	public:
		void OnAttach() override;
		void OnUpdate(float) override;
		void OnDetach() override;
		void OnEvent(Event &e) override;
		void OnGuiRender() override;
		void SetupDefaultCommands();

	private:
		bool OnWindowResize(WindowResizeEvent &e);
		bool OnKeyEvent(KeyEvent &e);

		void CreateWorld();
		void SaveWorld();
		void SaveWorldAs();
		void LoadWorld();
		void SetActiveWorld(const Ref<World> &world);

#pragma region GUI
		void ViewportGUI();
		void Viewport();

#pragma endregion
	};

} // namespace BHive
