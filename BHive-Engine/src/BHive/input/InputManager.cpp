#include "InputManager.h"
#include <glfw/glfw3.h>

namespace BHive
{
	void InputManager::add_input(uint32_t code, EventStatusCode action, ModCode mods)
	{
		mInputs[code] = action;
	}

	void InputManager::set_scroll(float x, float y)
	{
		mScroll = {x, y};
	}

	bool InputManager::is_pressed(uint32_t code) const
	{
		if (mInputs.contains(code))
			return mInputs.at(code) != EventStatus::RELEASE;

		return false;
	}

	EventStatusCode InputManager::get_input_state(uint32_t code) const
	{
		if (mInputs.contains(code))
			return mInputs.at(code);

		return EventStatus::RELEASE;
	}

	const glm::vec2 InputManager::get_mouse_pos() const
	{
		auto window = glfwGetCurrentContext();
		ASSERT(window);

		double x = 0.0, y = 0.0;
		glfwGetCursorPos(window, &x, &y);

		return {(float)x, (float)y};
	}

	const glm::vec2 InputManager::get_mouse_delta() const
	{
		static glm::vec2 previous_mouse_pos{0, 0};

		auto mouse_pos = get_mouse_pos();
		auto delta = mouse_pos - previous_mouse_pos;
		previous_mouse_pos = mouse_pos;

		return delta;
	}

	InputManager &InputManager::GetInputManager()
	{
		static InputManager manager;
		return manager;
	}
} // namespace BHive