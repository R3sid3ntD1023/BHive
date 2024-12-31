#pragma once

#include "reflection/Reflection.h"

namespace BHive
{
    class SceneRenderer;

    struct IRenderable
    {
        virtual void OnRender(SceneRenderer *renderer) = 0;

        REFLECTABLEV()
    };
} // namespace BHive
