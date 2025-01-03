#pragma once

#include "TAssetEditor.h"
#include "mesh/StaticMesh.h"
#include "cameras/EditorCamera.h"

namespace BHive
{
    class MeshEditor : public TAssetEditor<StaticMesh>
    {
    public:
        MeshEditor();

        virtual void OnWindowRender() override;
        virtual void OnEvent(Event &event) override;

        virtual const char *GetFileDialogFilter() { return "\0"; }

        virtual std::filesystem::path GetAssetPath(const std::filesystem::path &path) const override { return path.parent_path() / (path.stem().string() + ".meta"); }

    private:
        EditorCamera mCamera;
        Ref<class SceneRenderer> mRenderer;
        glm::vec2 mViewportSize{300, 300};
        glm::vec2 mViewportPanelSize{300, 300};
    };
}