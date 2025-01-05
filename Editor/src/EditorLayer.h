#pragma once

#include "BHive.h"
#include "cameras/EditorCamera.h"
#include <rttr/library.h>

namespace BHive
{
    class World;
    class SceneRenderer;

    enum EEditorMode
    {
        EDIT,
        PLAY,
        SIMULATE
    };

    class EditorLayer : public Layer
    {
    public:
        EditorLayer() = default;

        virtual void OnAttach() override;
        virtual void OnDetach() override;
        virtual void OnEvent(Event &event) override;
        virtual void OnUpdate(float dt) override;
        virtual void OnGuiRender(float) override;

    private:
        bool OnKeyEvent(KeyEvent &event);

        void FileMenu();
        void WindowMenu();

        void ShowSceneHierarchyPanel();
        void ShowPropertiesPanel();
        void ShowDetailsPanel();
        void ShowViewport();
        void ShowPerformancePanel();
        void ShowContentBrowser();
        void ShowAssetManagerPanel();
        void ShowRenderSettings();
        void ShowInputWindow();
        void ShowProjectWindow();
        void ViewportToolbar();

        void NewWorld();
        void SaveWorld();
        void SaveWorldAs();
        void OpenWorld(const std::filesystem::path &path = "");

        void OnWorldPlay();
        void OnWorldStop();
        void OnWorldSimulate();

    private:
        Ref<World> mEditorWorld;
        Ref<World> mActiveWorld;

        Ref<SceneRenderer> mSceneRenderer;
        EditorCamera mEditorCamera;

        EEditorMode mEditorMode = EEditorMode::EDIT;

    private:
        Ref<class SceneHierarchyPanel> mSceneHierarchyPanel;
        Ref<class PropertiesPanel> mPropertiesPanel;
        Ref<class ContentBrowserPanel> mContentBrowser;
        Ref<class AssetManagerBase> mAssetManager;
        Ref<class DetailsPanel> mDetailsPanel;

        glm::vec2 mViewportPanelSize{0};
        glm::uvec2 mViewportSize{0};

        bool mIsHovered{false};
        bool mIsFocused{false};

        bool mShowAssetManagerAssets{false};
        bool mShowRenderSettings{false};
        bool mShowInputSettings{false};
        bool mShowProjectWindow{false};
        bool mSnapEnabled{false};
        bool mIsTitleBarHovered{false};

        std::filesystem::path mCurrentWorldPath;
        EventDelegateHandle mLogEventHandle;

        int32_t mGizmoOperation = 7;
        int32_t mGizmoMode = 0;
        glm::vec2 mViewportBounds[2];
    };
} // namespace BHive
