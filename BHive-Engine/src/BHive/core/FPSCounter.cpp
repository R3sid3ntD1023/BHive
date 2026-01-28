#include "FPSCounter.h"

namespace BHive
{
	FPSCounter::FPSCounter()
	{
		mLastTime = std::chrono::high_resolution_clock::now();
	}

	void FPSCounter::Frame()
	{

		auto current = std::chrono::high_resolution_clock::now();
		mFrameCount++;
		auto elapsed = (current - mLastTime);
		auto seconds = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count() / 1000.f;

		if (seconds >= 1.0)
		{
			mFPS = (float)mFrameCount;
			mFrameCount = 0;
			mLastTime = current;
		}
	}

	FPSCounter::operator float() const
	{
		return mFPS;
	}

	FPSCounter &FPSCounter::Get()
	{
		static FPSCounter counter;
		return counter;
	}
} // namespace BHive
