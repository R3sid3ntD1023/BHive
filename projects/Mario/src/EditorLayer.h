#pragma once

#include "core/Layer.h"
#include "objects/World.h"
#include "core/events/ApplicationEvents.h"
#include "editor/SceneHeirarchyPanel.h"
#include "editor/PropertiesPanel.h"
#include "editor/EditorContentBrowser.h"
#include "gfx/cameras/EditorCamera.h"
#include "asset/EditorAssetManager.h"

namespace BHive
{
	class Framebuffer;

	class EditorLayer : public Layer
	{
	private:
		Ref<Framebuffer> mFramebuffer;
		Ref<World> mEditorWorld;
		glm::ivec2 mViewportSize{};
		Ref<SceneHierarchyPanel> mSceneHeirarchyPanel;
		Ref<PropertiesPanel> mPropertiesPanel;
		Ref<EditorContentBrowser<EditorAssetManager>> mContentBrowser;
		EditorCamera mEditorCamera;

		EditorAssetManager mAssetManager{RESOURCE_PATH};

	public:
		void OnAttach() override;
		void OnUpdate(float) override;
		void OnDetach() override;
		void OnEvent(Event &e) override;
		void OnGuiRender() override;

	private:
		bool OnWindowResize(WindowResizeEvent &e);
	};

} // namespace BHive
