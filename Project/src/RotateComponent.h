#pragma once

#include "scene/ActorComponent.h"

namespace BHive
{
    struct RotateComponent : public ActorComponent
    {
        /* data */

        virtual void OnUpdate(float dt) override;

        float mRotateSpeed = 1.0f;

        void Serialize(StreamWriter &ar) const;
        void Deserialize(StreamReader &ar);

        REFLECTABLEV(ActorComponent)
    };

    REFLECT_EXTERN(RotateComponent)

} // namespace BHive
