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
		FOnWindowEvent WindowEvent;

	public:
		void OnWindowClose();
		void OnWindowResize(int w, int h);
		void OnKeyEvent(int key, int scancode, int action, int mods);
		void OnKeyTypedEvent(unsigned int codepoint);
		void OnMouseButton(int button, int action, int mods);
		void OnMouseScroll(double x, double y);
		void OnMouseMoved(double x, double y);
		void OnFramebufferResized(int w, int h);

		static void OnJoyStickConnected(int joystick, int status);
		static glm::vec2 GetJoyStickAxes(JoyStickAxisCode axis);
		static uint8_t GetJoyStickButton(JoyStickButtonCode button);

	private:
		static inline int sJoyStickID = -1;
		static inline glm::vec2 sJoyStickDeadZones[3];
	};
} // namespace BHive