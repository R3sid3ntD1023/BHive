#pragma once

#include "core/Core.h"

struct GLFWwindow;

namespace BHive
{
	class BHIVE_API GraphicsContext
	{
	public:
		GraphicsContext(GLFWwindow *window);
		virtual ~GraphicsContext() = default;

		virtual void Init();
		virtual void SwapBuffers();

	private:
		GLFWwindow *mWindowHandle;
	};
} // namespace BHive