#pragma once

#include "Event.h"

namespace BHive
{
	class BHIVE_API WindowCloseEvent : public Event
	{
		EVENT(WindowCloseEvent, EventCategoryApplication)

	public:
		WindowCloseEvent() = default;
	};

	class BHIVE_API WindowResizeEvent : public Event
	{

		EVENT(WindowResizeEvent, EventCategoryApplication)

	public:
		WindowResizeEvent(unsigned x, unsigned y)
			: x(x),
			  y(y)
		{
		}

		unsigned x = 0;
		unsigned y = 0;
	};
} // namespace BHive