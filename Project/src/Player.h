#pragma once

#include "scene/Entity.h"
#include "IInputActor.h"

namespace BHive
{
    class InputComponent;

    class Player : public Entity, public IInputActor
    {
    public:
        Player();

        virtual Ref<InputComponent> GetInputComponent() { return mInputComponent; }

    private:
        Ref<InputComponent> mInputComponent;

        REFLECTABLEV(Entity)
    };

    REFLECT_EXTERN(Player)
} // namespace BHive
