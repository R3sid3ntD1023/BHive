#pragma once

#include "core/Core.h"
#include "events/Event.h"
#include "events/Input.h"
#include "events/JoyStickCodes.h"
#include <glm/glm.hpp>

struct GLFWwindow;

namespace BHive
{
	class WindowInput
	{
	public:
		static BHIVE inline std::function<void(Event &)> mEvent;

	public:
		static BHIVE void OnWindowClose();
		static BHIVE void OnWindowResize(int w, int h);
		static BHIVE void OnKeyEvent(int key, int scancode, int action, int mods);
		static BHIVE void OnKeyTypedEvent(unsigned int codepoint);
		static BHIVE void OnMouseButton(int button, int action, int mods);
		static BHIVE void OnMouseScroll(double x, double y);
		static BHIVE void OnMouseMoved(double x, double y);

		static void OnJoyStickConnected(int joystick, int status);
		static glm::vec2 GetJoyStickAxes(JoyStickAxisCode axis);
		static InputActionCode GetJoyStickButton(JoyStickButtonCode button);

		static BHIVE const Input &GetInput() { return sInput; }

	private:
		static inline Input sInput;
		static inline int sJoyStickID = -1;
		static inline glm::vec2 sJoyStickDeadZones[3];
	};
}