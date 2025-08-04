#include "ImWindowSystem.h"

namespace BHive
{
	void ImWindowSystem::Update()
	{
		std::remove_if(mWindows.begin(), mWindows.end(), [](const Ref<ImWindow> &window) { return window->ShouldClose(); });

		for (auto &window : mWindows)
		{
			window->Update();
		}
	}

	void ImWindowSystem::ProcessEvents(Event &event)
	{
		for (auto &window : mWindows)
		{
			if (event.mHandled)
				break;

			if (window->IsFocused())
				window->OnEvent(event);
		}
	}

	void ImWindowSystem::EmplaceWindow(const Ref<ImWindow> &window)
	{
		mWindows.emplace_back(window);
	}

	void ImWindowSystem::Clear()
	{
		mWindows.clear();
	}

} // namespace BHive
