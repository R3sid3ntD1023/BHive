// #pragma once

#include "scene/ActorComponent.h"
#include "input/InputValue.h"
#include "sprite/FlipBook.h"

namespace BHive
{
    struct MoveComponent : public ActorComponent
    {
        virtual void OnBegin() override;

        void Jump(const BHive::InputValue &value);
        void Move(const BHive::InputValue &value);
        void UpdateState();

        float mSpeed = 100.f;
        float mJumpForce = 300.f;
        std::unordered_map<std::string, TAssetHandle<FlipBook>> mStates;

        void Serialize(StreamWriter &ar) const;
        void Deserialize(StreamReader &ar);

        REFLECTABLEV(ActorComponent)
    };

    REFLECT_EXTERN(MoveComponent)
}