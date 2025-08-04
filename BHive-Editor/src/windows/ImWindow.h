#pragma once

#include "core/events/Event.h"

namespace BHive
{
	struct ImWindow
	{
		virtual void Update() = 0;

		virtual bool ShouldClose() const = 0;

		virtual bool IsFocused() const = 0;

		virtual void OnEvent(Event &e) {}
	};

} // namespace BHive
