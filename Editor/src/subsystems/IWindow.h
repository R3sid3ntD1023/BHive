#pragma once

#include "events/Event.h"

namespace BHive
{
    struct IWindow
    {
        /* data */
        virtual void OnUpdate() = 0;

        virtual bool ShouldClose() const = 0;

        virtual bool IsFocused() const = 0;

        virtual void OnEvent(Event &event) {};
    };

} // namespace BHive
