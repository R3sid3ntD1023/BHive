#include "Time.h"
#include <glfw/glfw3.h>

namespace BHive
{
	float Time::Get()
	{
		return (float)glfwGetTime();
	}

	float Time::GetDeltaTime()
	{
		static float lasttime = 0;
		float time = Time::Get();
		float deltaTime = time - lasttime;
		lasttime = time;

		return deltaTime;
	}

	float Time::GetFPS()
	{
		static float fps = 0;
		static int frames = 0;
		static float last_time = 0.0f;
		float time = Time::Get();
		float delta = time - last_time;

		if (delta >= 1.0f)
		{
			fps = (float)frames / delta;
			frames = 0;
			last_time = time;
		}

		frames++;

		return fps;
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
		return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - mStart).count() * .001f * .001f * .001f;
	}

	float Timer::ElaspedMillis()
	{
		return Elasped() * 1000.0f;
	}
}