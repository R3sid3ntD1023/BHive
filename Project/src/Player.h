#pragma once

#include "scene/Actor.h"
#include "IInputActor.h"

namespace BHive
{
    class InputComponent;

    class Player : public Actor
    {
    public:
        Player();

        virtual Ref<InputComponent> GetInputComponent() { return mInputComponent; }

    private:
        Ref<InputComponent> mInputComponent;

        REFLECTABLEV(Actor)
    };

    REFLECT_EXTERN(Player)
} // namespace BHive
