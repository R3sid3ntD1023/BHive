#pragma once

#include "core/Core.h"
#include "events/Event.h"
#include "events/JoyStickCodes.h"
#include "core/delegates/EventDelegate.h"
#include "core/delegates/MultiEventDelegate.h"
#include <glm/glm.hpp>

struct GLFWwindow;

namespace BHive
{
	DECLARE_MULTI_EVENT(FOnWindow, Event &);

	class WindowInput
	{
	public:
		static inline FOnWindowEvent WindowEvent;

	public:
		static void OnWindowClose(GLFWwindow *);
		static void OnWindowResize(GLFWwindow *, int w, int h);
		static void OnWindowFocused(GLFWwindow *, int focused);
		static void OnKeyEvent(GLFWwindow *, int key, int scancode, int action, int mods);
		static void OnKeyTypedEvent(GLFWwindow *, unsigned int codepoint);
		static void OnMouseButton(GLFWwindow *, int button, int action, int mods);
		static void OnMouseScroll(GLFWwindow *, double x, double y);
		static void OnMouseMoved(GLFWwindow *, double x, double y);
		static void OnFramebufferResized(GLFWwindow *, int w, int h);
		static void OnJoyStick(int joyStick, int status);

		static void OnJoyStickConnected(int joystick, int status);
		static glm::vec2 GetJoyStickAxes(JoyStickAxisCode axis);
		static uint8_t GetJoyStickButton(JoyStickButtonCode button);

		static void RegisterCallbacks(GLFWwindow *window, void *userPointer = nullptr);

		static GLFWwindow *GetFocusedWindow();

	private:
		static inline int sJoyStickID = -1;
		static inline glm::vec2 sJoyStickDeadZones[3];
		static inline GLFWwindow *sFocusedWindow = nullptr;
	};
} // namespace BHive