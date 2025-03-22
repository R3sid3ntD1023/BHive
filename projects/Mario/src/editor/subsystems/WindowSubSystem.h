#pragma once

#include "core/Core.h"
#include "editor/IWindow.h"

namespace BHive
{
	struct WindowSubSystem
	{
		~WindowSubSystem();
		void UpdateWindows();
		void ProcessEvents(Event &event);

		template <typename T, typename... TArgs>
		Ref<T> CreateWindow(TArgs &&...args)
		{
			static_assert(std::is_base_of_v<IWindow, T>, "T must be derived from IWindow");

			auto window = CreateRef<T>(std::forward<TArgs>(args)...);
			AddWindow(window);
			return window;
		}

		void AddWindow(const Ref<IWindow> &window);

		void Clear();

	private:
		std::unordered_set<Ref<IWindow>> mWindows;
	};
} // namespace BHive