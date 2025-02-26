#pragma once

#include "core/Core.h"

namespace BHive
{
    class InputComponent;

    struct IInputActor
    {
        virtual Ref<InputComponent> GetInputComponent() = 0;
    };

} // namespace BHive
