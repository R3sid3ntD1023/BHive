#pragma once

#include "scene/Component.h"

namespace BHive
{
    struct RotateComponent : public Component
    {
        /* data */

        virtual void OnBegin() override;
        virtual void OnUpdate(float dt) override;

        float mRotateSpeed = 1.0f;

        virtual void Save(cereal::JSONOutputArchive &ar) const override;

		virtual void Load(cereal::JSONInputArchive &ar) override;

        REFLECTABLEV(Component)
    };

    REFLECT_EXTERN(RotateComponent)

} // namespace BHive
