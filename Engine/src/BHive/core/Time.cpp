#include "Time.h"
#include <glfw/glfw3.h>

namespace BHive
{
	float Time::Get()
	{
		return (float)glfwGetTime();
	}

	Timer::Timer()
	{
		Reset();
	}

	void Timer::Reset()
	{
		mStart = std::chrono::high_resolution_clock::now();
	}

	float Timer::Elasped()
	{
		return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - mStart).count() * .001f * .001f *
			   .001f;
	}

	float Timer::ElaspedMillis()
	{
		return Elasped() * 1000.0f;
	}
} // namespace BHive