#include "WindowInput.h"
#include "events/ApplicationEvents.h"
#include "events/MouseEvents.h"
#include "events/KeyEvents.h"
#include "input/InputManager.h"
#include "Window.h"
#include "gfx/RenderCommand.h"
#include <glfw/glfw3.h>

namespace BHive
{
	void WindowInput::OnWindowClose(GLFWwindow *)
	{
		WindowCloseEvent event;
		WindowEvent.Broadcast(event);
	}

	void WindowInput::OnWindowResize(GLFWwindow *window, int w, int h)
	{
		if (w == 0 || h == 0)
			return;

		WindowResizeEvent event((unsigned)w, (unsigned)h);
		WindowEvent.Broadcast(event);
	}

	void WindowInput::OnWindowFocused(GLFWwindow *window, int focused)
	{
		if (focused)
		{
			sFocusedWindow = window;
		}
		else
		{
			sFocusedWindow = nullptr;
		}
	}

	void WindowInput::OnKeyEvent(GLFWwindow *, int key, int scancode, int action, int mods)
	{
		KeyEvent event((KeyCode)key, scancode, action, mods);
		WindowEvent.Broadcast(event);

		auto &im = InputManager::Get();
		im.OnKeyEvent(key, (EventStatusCode)action, (ModCode)mods);
	}

	void WindowInput::OnKeyTypedEvent(GLFWwindow *, unsigned int codepoint)
	{
		KeyTypedEvent event((KeyCode)codepoint);
		WindowEvent.Broadcast(event);
	}

	void WindowInput::OnMouseButton(GLFWwindow *, int button, int action, int mods)
	{
		MouseButtonEvent event((MouseCode)button, action, mods);
		WindowEvent.Broadcast(event);

		auto &im = InputManager::Get();
		im.OnMouseEvent(button, (EventStatusCode)action, (ModCode)mods);
	}

	void WindowInput::OnMouseScroll(GLFWwindow *, double x, double y)
	{
		MouseScrolledEvent event((float)x, (float)y);
		WindowEvent.Broadcast(event);

		auto &im = InputManager::Get();
		im.OnScrollEvent((float)x, (float)y);
	}

	void WindowInput::OnMouseMoved(GLFWwindow *, double x, double y)
	{
		MouseMovedEvent event((float)x, (float)y);
		WindowEvent.Broadcast(event);

		auto &im = InputManager::Get();
		im.OnMouseMove(float(x), float(y));
	}

	void WindowInput::OnFramebufferResized(GLFWwindow *, int w, int h)
	{
		WindowResizeEvent event(w, h);
		WindowEvent.Broadcast(event);
	}

	void WindowInput::OnJoyStick(int joyStick, int status)
	{
		OnJoyStickConnected(joyStick, status);

		for (int i = 0; i < GLFW_JOYSTICK_LAST; i++)
		{
			auto present = glfwJoystickPresent(i);
			if (present)
			{
				OnJoyStickConnected(i, GLFW_CONNECTED);
				break;
			}
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

	uint8_t WindowInput::GetJoyStickButton(JoyStickButtonCode button)
	{
		if (sJoyStickID == -1)
			return 0;

		int count = 0;
		auto buttons = glfwGetJoystickButtons(sJoyStickID, &count);

		return buttons[(int)button];
	}

	void WindowInput::RegisterCallbacks(GLFWwindow *window, void *userPointer)
	{
		glfwSetWindowUserPointer(window, userPointer);

		glfwSetWindowCloseCallback(window, OnWindowClose);

		glfwSetWindowSizeCallback(window, OnWindowResize);

		glfwSetWindowPosCallback(window, OnWindowResize);

		glfwSetKeyCallback(window, OnKeyEvent);

		glfwSetMouseButtonCallback(window, OnMouseButton);

		glfwSetScrollCallback(window, OnMouseScroll);

		glfwSetCursorPosCallback(window, OnMouseMoved);

		glfwSetCharCallback(window, OnKeyTypedEvent);

		glfwSetFramebufferSizeCallback(window, OnFramebufferResized);

		glfwSetWindowFocusCallback(window, OnWindowFocused);

		glfwSetJoystickCallback(OnJoyStick);
	}

	GLFWwindow *WindowInput::GetFocusedWindow()
	{
		if (RenderCommand::GetAPI() == RendererAPI::Opengl)
			return glfwGetCurrentContext();

		return sFocusedWindow;
	}
} // namespace BHive