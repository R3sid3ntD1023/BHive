#include "Time.h"
#include <glfw/glfw3.h>

namespace BHive
{
	float Time::Raw()
	{
		return (float)glfwGetTime();
	}

	float Time::DeltaTime()
	{
		return mDeltaTime;
	}

	void Time::Update()
	{
		mTime = Raw();
		mDeltaTime = mTime - mLastTime;
		mLastTime = mTime;
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