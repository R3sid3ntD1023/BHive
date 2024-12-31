#pragma once

#include "Event.h"
#include "KeyCodes.h"

namespace BHive
{
	class BHIVE KeyEvent : public Event
	{
		EVENT(KeyEvent, EventCategoryKeyboard | EventCategoryInput)

	public:
		KeyEvent(KeyCode key, int scancode, InputActionCode action, ModCode mods)
			:Key(key), ScanCode(scancode), Action(action), Mods(mods)
		{}

		KeyCode Key;
		int ScanCode;
		InputActionCode Action;
		ModCode Mods;
	};

	class BHIVE KeyTypedEvent : public Event
	{
		EVENT(KeyTypedEvent, EventCategoryKeyboard | EventCategoryInput)

	public:
		KeyTypedEvent(KeyCode key)
			:Key(key)
		{}

		KeyCode Key;
	};
}