#pragma once

#include "core/Layer.h"
#include "objects/World.h"
#include "core/events/ApplicationEvents.h"
#include "core/events/KeyEvents.h"
#include "editor/SceneHeirarchyPanel.h"
#include "editor/PropertiesPanel.h"
#include "editor/EditorContentBrowser.h"
#include "gfx/cameras/EditorCamera.h"
#include "asset/EditorAssetManager.h"
#include "Command.h"

namespace BHive
{
	class Framebuffer;

	class EditorLayer : public Layer
	{
	private:
		Ref<Framebuffer> mFramebuffer;
		Ref<World> mEditorWorld;

		Ref<SceneHierarchyPanel> mSceneHeirarchyPanel;
		Ref<PropertiesPanel> mPropertiesPanel;
		Ref<EditorContentBrowser<EditorAssetManager>> mContentBrowser;
		EditorCamera mEditorCamera;

		EditorAssetManager mAssetManager{RESOURCE_PATH, "AssetRegistry.json"};

		glm::ivec2 mViewportSize{};
		glm::ivec2 mViewportBounds[2] = {};

		uint32_t mGizmoOperation = 0;
		uint32_t mGizmoMode = 0;
		bool mSnappingEnabled{false};

		std::unordered_map<FCommand, std::function<void()>> mCommands;

		std::filesystem::path mCurrentWorldPath = "";

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
	};

} // namespace BHive
