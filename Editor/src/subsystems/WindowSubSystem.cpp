#include "WindowSubSystem.h"

namespace BHive
{
	void WindowSubSystem::UpdateWindows()
	{
		std::unordered_set<Ref<IWindow>> removed_windows;

		for (auto &window : mWindows)
		{
			window->OnUpdate();

			if (window->ShouldClose())
				removed_windows.insert(window);
		}

		for (auto it : removed_windows)
		{
			mWindows.erase(it);
		}
	}

	void WindowSubSystem::ProcessEvents(Event &event)
	{
		for (auto &window : mWindows)
		{
			if (event.mHandled)
				break;

			if (window->IsFocused())
				window->OnEvent(event);
		}
	}

	void WindowSubSystem::AddWindow(const Ref<IWindow> &window)
	{
		mWindows.insert(window);
	}

	void WindowSubSystem::Clear()
	{
		mWindows.clear();
	}

} // namespace BHive
