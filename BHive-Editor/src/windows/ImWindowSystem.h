#pragma once

#include "core/Core.h"
#include "windows/ImWindow.h"

namespace BHive
{
	struct ImWindowSystem
	{
		void Update();

		void ProcessEvents(Event &event);

		template <typename T, typename... TArgs>
		Ref<T> ConstructWindow(TArgs &&...args)
		{
			static_assert(std::is_base_of_v<ImWindow, T>, "T must be derived from IWindow");

			auto window = CreateRef<T>(std::forward<TArgs>(args)...);
			EmplaceWindow(window);
			return window;
		}

		void EmplaceWindow(const Ref<ImWindow> &window);

		void Clear();

	private:
		std::vector<Ref<ImWindow>> mWindows;
	};
} // namespace BHive