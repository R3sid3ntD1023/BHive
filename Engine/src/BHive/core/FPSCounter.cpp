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
		auto elapsed = current - mLastTime;

		if (std::chrono::duration_cast<std::chrono::seconds>(elapsed) >= std::chrono::seconds{1})
		{
			// mFPS = mFrameCount / elapsed.count();
			mFPS = mFrameCount / std::chrono::duration<double>(elapsed).count();
			mFrameCount = 0;
			mLastTime = current;
		}
	}

	FPSCounter::operator float() const
	{
		return mFPS;
	}
} // namespace BHive
