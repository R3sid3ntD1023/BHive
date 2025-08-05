#include "ImWindowSystem.h"

namespace BHive
{
	void ImWindowSystem::Update()
	{
		auto windows_count = std::erase_if(mWindows, [](const Ref<ImWindow> &window) { return window->ShouldClose(); });
		for (auto &window : mWindows)
		{
			window->OnUpdate();
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
		mWindows.insert(window);
	}

	void ImWindowSystem::Clear()
	{
		mWindows.clear();
	}

} // namespace BHive
