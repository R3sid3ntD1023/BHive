#pragma once

#include "TAssetEditor.h"
#include "mesh/SkeletalAnimation.h"

namespace BHive
{
    class AnimationEditor : public TAssetEditor<SkeletalAnimation>
    {
    public:
        virtual void OnWindowRender();

        virtual const char *GetFileDialogFilter() { return "Animation (*.animation)\0*.animation\0"; };
    };
}