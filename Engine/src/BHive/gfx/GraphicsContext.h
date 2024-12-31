#pragma once

#include "core/Core.h"

struct GLFWwindow;

namespace BHive
{
	class BHIVE GraphicsContext
	{
	public:
		virtual ~GraphicsContext() = default;

		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;

		static Scope<GraphicsContext> Create(void* window);
	};
}