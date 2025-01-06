#pragma once

#include "scene/Component.h"

namespace BHive
{
    struct RotateComponent : public Component
    {
        /* data */

        virtual void OnUpdate(float dt) override;

        float mRotateSpeed = 1.0f;

        void Serialize(StreamWriter &ar) const;
        void Deserialize(StreamReader &ar);

        REFLECTABLEV(Component)
    };

    REFLECT_EXTERN(RotateComponent)

} // namespace BHive
