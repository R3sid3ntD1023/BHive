#pragma once

#include "reflection/Reflection.h"

namespace BHive
{
    struct ITickable
    {
        virtual bool IsTickEnabled() const = 0;

        virtual void SetTickEnabled(bool) = 0;

        REFLECTABLEV()
    };

    REFLECT(ITickable)
    {
        BEGIN_REFLECT(ITickable)
        REFLECT_PROPERTY("TickEnabled", IsTickEnabled, SetTickEnabled);
    }
} // namespace BHive
