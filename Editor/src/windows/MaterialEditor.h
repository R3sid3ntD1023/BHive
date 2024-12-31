#pragma once

#include "TAssetEditor.h"
#include "material/Material.h"
#include "gfx/Camera.h"

namespace BHive
{
    class MaterialEditor : public TAssetEditor<Material>
    {
    public:
        MaterialEditor();

    protected:
        virtual void OnWindowRender();

        virtual const char *GetFileDialogFilter() { return "Material (*.material)\0*.material\0"; };

    private:
        Camera mCamera;
        Ref<class SceneRenderer> mRenderer;
        static inline Ref<class StaticMesh> mSphere;
        glm::vec2 mViewportSize{300, 300};
        glm::vec2 mViewportPanelSize{300, 300};
    };
} // namespace BHive
