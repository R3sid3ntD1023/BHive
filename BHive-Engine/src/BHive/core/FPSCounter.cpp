#include "FPSCounter.h"

namespace BHive
{
	FPSCounter::FPSCounter()
	{
		mLastTime = std::chrono::high_resolution_clock::now();
	}

	void FPSCounter::Frame()
	{

		auto now = std::chrono::high_resolution_clock::now();
		auto dt = std::chrono::duration<double>(now - mLastTime).count();
		mLastTime = now;

		double fps = 1.0 / dt;
		mFPS = mFPS * 0.9 + fps * 0.1;
	}

	FPSCounter::operator float() const
	{
		return (float)mFPS;
	}

	FPSCounter &FPSCounter::Get()
	{
		static FPSCounter counter;
		return counter;
	}
} // namespace BHive
