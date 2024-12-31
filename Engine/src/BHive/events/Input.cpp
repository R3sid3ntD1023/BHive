#include "Input.h"
#include <glfw/glfw3.h>

namespace BHive
{
	void Input::add_input(uint32_t code, InputActionCode action, ModCode mods)
	{
		mInputs[code] = action;

		// LOG_TRACE("INPUT::TRACE Added Key {}, Action {}", code, action);
	}

	void Input::set_scroll(float x, float y)
	{
		mScroll = {x, y};
	}

	bool Input::is_pressed(uint32_t code) const
	{
		if (mInputs.contains(code))
			return mInputs.at(code) != InputAction::RELEASE;

		return false;
	}

	InputActionCode Input::get_input_state(uint32_t code) const
	{
		if (mInputs.contains(code))
			return mInputs.at(code);

		return InputAction::RELEASE;
	}

	const glm::vec2 Input::get_mouse_pos() const
	{
		auto window = glfwGetCurrentContext();
		ASSERT(window);

		double x = 0.0, y = 0.0;
		glfwGetCursorPos(window, &x, &y);

		return {(float)x, (float)y};
	}

	const glm::vec2 Input::get_mouse_delta() const
	{
		static glm::vec2 previous_mouse_pos{0, 0};

		auto mouse_pos = get_mouse_pos();
		auto delta = mouse_pos - previous_mouse_pos;
		previous_mouse_pos = mouse_pos;

		return delta;
	}
}