#pragma once

#include "Event.h"
#include "MouseCodes.h"

namespace BHive
{
	class BHIVE_API MouseButtonEvent : public Event
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

	class BHIVE_API MouseScrolledEvent : public Event
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

	class BHIVE_API MouseMovedEvent : public Event
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