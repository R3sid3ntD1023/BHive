#include "WindowSubSystem.h"

namespace BHive
{
	WindowSubSystem::~WindowSubSystem()
	{
	}

	void WindowSubSystem::UpdateWindows()
	{
		for (auto &window : mWindows)
		{
			window->OnUpdate();
		}

		// std::remove_if(mWindows.begin(), mWindows.end(), [](const Ref<IWindow> &window) { return window->ShouldClose();
		// });
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
