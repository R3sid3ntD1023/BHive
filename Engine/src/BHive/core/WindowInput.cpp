#include "WindowInput.h"
#include "events/ApplicationEvents.h"
#include "events/InputEvents.h"
#include "events/MouseEvents.h"
#include <glfw/glfw3.h>

namespace BHive
{
	void WindowInput::OnWindowClose()
	{
		if (mEvent)
		{
			WindowCloseEvent event;
			mEvent(event);
		}
	}

	void WindowInput::OnWindowResize(int w, int h)
	{
		if (mEvent)
		{
			WindowResizeEvent event((unsigned)w, (unsigned)h);
			mEvent(event);
		}
	}

	void WindowInput::OnKeyEvent(int key, int scancode, int action, int mods)
	{
		if (mEvent)
		{
			sInput.add_input((KeyCode)key, (InputActionCode)action, (ModCode)mods);
			KeyEvent event((KeyCode)key, scancode, (InputActionCode)action, (ModCode)mods);
			mEvent(event);
		}
	}

	void WindowInput::OnKeyTypedEvent(unsigned int codepoint)
	{
		if (mEvent)
		{
			KeyTypedEvent event((KeyCode)codepoint);
			mEvent(event);
		}
	}

	void WindowInput::OnMouseButton(int button, int action, int mods)
	{
		if (mEvent)
		{
			sInput.add_input((MouseCode)button, (InputActionCode)action, (ModCode)mods);
			MouseButtonEvent event((MouseCode)button, (InputActionCode)action, (ModCode)mods);
			mEvent(event);

			// LOG_TRACE("Key : {}, Action : {}", button, action);
		}
	}

	void WindowInput::OnMouseScroll(double x, double y)
	{
		if (mEvent)
		{
			sInput.set_scroll((float)x, (float)y);
			MouseScrolledEvent event((float)x, (float)y);
			mEvent(event);
		}
	}

	void WindowInput::OnMouseMoved(double x, double y)
	{
		if (mEvent)
		{
			MouseMovedEvent event((float)x, (float)y);
			mEvent(event);
		}
	}

	void WindowInput::OnJoyStickConnected(int joystick, int status)
	{
		switch (status)
		{
		case GLFW_CONNECTED:
		{
			LOG_TRACE("Connected Joystick {}", joystick);
			sJoyStickID = joystick;

			int count;
			auto axes = glfwGetJoystickAxes(sJoyStickID, &count);

			sJoyStickDeadZones[0] = {axes[GLFW_GAMEPAD_AXIS_LEFT_X], axes[GLFW_GAMEPAD_AXIS_LEFT_Y]};
			sJoyStickDeadZones[1] = {axes[GLFW_GAMEPAD_AXIS_RIGHT_X], axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]};
			sJoyStickDeadZones[2] = {axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER], axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER]};
			break;
		}
		case GLFW_DISCONNECTED:
		{
			LOG_TRACE("Disconnected Joystick {}", joystick);
			sJoyStickID = -1;
			break;
		}
		default:
			break;
		}
	}

	glm::vec2 WindowInput::GetJoyStickAxes(JoyStickAxisCode axis)
	{
		if (sJoyStickID == -1)
			return {0, 0};

		int count = 0;
		auto axes = glfwGetJoystickAxes(sJoyStickID, &count);

		glm::vec2 value{0, 0};

		switch (axis)
		{
		case JoyStickAxis::LEFT:
		{
			value = {axes[GLFW_GAMEPAD_AXIS_LEFT_X], axes[GLFW_GAMEPAD_AXIS_LEFT_Y]};
			value -= sJoyStickDeadZones[0];
			break;
		}
		case JoyStickAxis::RIGHT:
		{
			value = {axes[GLFW_GAMEPAD_AXIS_RIGHT_X], axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]};
			value -= sJoyStickDeadZones[1];
			break;
		}
		case JoyStickAxis::TRIGGERS:
		{
			value = {axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER], axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER]};
			value -= sJoyStickDeadZones[2];
			break;
		}
		default:
			break;
		}

		return value;
	}

	InputActionCode WindowInput::GetJoyStickButton(JoyStickButtonCode button)
	{
		if (sJoyStickID == -1)
			return InputAction::RELEASE;

		int count = 0;
		auto buttons = glfwGetJoystickButtons(sJoyStickID, &count);

		return buttons[(int)button];
	}

}