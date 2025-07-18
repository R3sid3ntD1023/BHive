#pragma once

#include "Event.h"
#include "MouseCodes.h"

namespace BHive
{
	class BHIVE MouseButtonEvent : public Event
	{
		EVENT(MouseButtonEvent, EventCategoryMouseButton | EventCategoryInput)

	public:
		MouseButtonEvent(MouseCode button, int action, int mods)
			: Button(button),
			  Action(action),
			  Mods(mods)
		{
		}

		MouseCode Button;
		int Action;
		int Mods;
	};

	class BHIVE MouseScrolledEvent : public Event
	{
		EVENT(MouseScrolledEvent, EventCategoryMouse | EventCategoryInput)

	public:
		MouseScrolledEvent(float _x, float _y)
			: x(_x),
			  y(_y)
		{
		}

		float x, y;
	};

	class MouseMovedEvent : public Event
	{
		EVENT(MouseMovedEvent, EventCategoryMouse | EventCategoryInput)

	public:
		MouseMovedEvent(float _x, float _y)
			: x(_x),
			  y(_y)
		{
		}

		float x, y;
	};
} // namespace BHive