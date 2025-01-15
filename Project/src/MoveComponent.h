// #pragma once

#include "scene/Component.h"
#include "input/InputValue.h"
#include "sprite/FlipBook.h"

namespace BHive
{
    struct MoveComponent : public Component
    {
        virtual void OnBegin() override;

        void Jump(const BHive::InputValue &value);
        void Move(const BHive::InputValue &value);
        void UpdateState();

        float mSpeed = 100.f;
        float mJumpForce = 300.f;
        std::unordered_map<std::string, TAssetHandle<FlipBook>> mStates;

        virtual void Save(cereal::BinaryOutputArchive &ar) const override;

		virtual void Load(cereal::BinaryInputArchive &ar) override;

        REFLECTABLEV(Component)
    };

    REFLECT_EXTERN(MoveComponent)
}