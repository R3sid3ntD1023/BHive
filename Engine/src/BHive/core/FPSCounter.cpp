#include "FPSCounter.h"

namespace BHive
{
	FPSCounter::FPSCounter()
	{
		mLastTime = std::chrono::high_resolution_clock::now();
	}

	void FPSCounter::Frame()
	{
		mFrameCount++;
		auto current = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed = current - mLastTime;

		if (elapsed.count() > 1.0)
		{
			mFPS = mFrameCount / elapsed.count();
			mFrameCount = 0;
			mLastTime = current;
		}
	}

	FPSCounter::operator float() const
	{
		return mFPS;
	}
} // namespace BHive
