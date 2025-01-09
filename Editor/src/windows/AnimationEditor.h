#pragma once

#include "TAssetEditor.h"
#include "mesh/SkeletalAnimation.h"

namespace BHive
{
    class AnimationEditor : public TAssetEditor<SkeletalAnimation>
    {
    public:
        virtual void OnWindowRender();
    };
}