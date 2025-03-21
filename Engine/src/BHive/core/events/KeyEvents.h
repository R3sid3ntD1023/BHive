#pragma once

#include "Event.h"
#include "KeyCodes.h"
#include "ModCode.h"
#include "EventStatus.h"

namespace BHive
{
	class BHIVE KeyEvent : public Event
	{
		EVENT(KeyEvent, EventCategoryKeyboard | EventCategoryInput)

	public:
		KeyEvent(int key, int scancode, int action, int mods)
			: Key(key),
			  ScanCode(scancode),
			  Action(action),
			  Mods(mods)
		{
		}

		int Key;
		int ScanCode;
		int Action;
		int Mods;
	};

	class BHIVE KeyTypedEvent : public Event
	{
		EVENT(KeyTypedEvent, EventCategoryKeyboard | EventCategoryInput)

	public:
		KeyTypedEvent(KeyCode key)
			: Key(key)
		{
		}

		KeyCode Key;
	};
} // namespace BHive