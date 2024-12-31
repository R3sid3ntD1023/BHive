#pragma once

#include "core/Core.h"
#include "IWindow.h"

namespace BHive
{
    struct WindowSubSystem
    {

        void UpdateWindows();
        void ProcessEvents(Event &event);

        template <typename T, typename... TArgs>
        Ref<T> AddWindow(TArgs &&...args)
        {
            static_assert(std::is_base_of_v<IWindow, T>, "T must be derived from IWindow");

            auto window = CreateRef<T>(std::forward<TArgs>(args)...);
            AddWindow(window);
            return window;
        }

        void AddWindow(const Ref<IWindow> &window);

    private:
        std::unordered_set<Ref<IWindow>> mWindows;
    };
}