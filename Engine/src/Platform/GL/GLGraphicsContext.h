#pragma once

#include "gfx/GraphicsContext.h"

struct GLFWwindow;

namespace BHive
{
	class GLGraphicsContext : public GraphicsContext
	{
	public:
		GLGraphicsContext(void* window);
		~GLGraphicsContext();

		void Init();
		void SwapBuffers();

	private:
		GLFWwindow* mWindowHandle;
	};
}