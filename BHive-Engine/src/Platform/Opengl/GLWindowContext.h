#pragma once

#include "gfx/WindowContext.h"

struct GLFWwindow;

namespace BHive
{
	class GLWindowContext : public WindowContext
	{
	public:
		GLWindowContext(Window *window);

		void Init() override;

		void SwapBuffers() override;

	private:
		GLFWwindow *mWindowHandle;
	};
} // namespace BHive