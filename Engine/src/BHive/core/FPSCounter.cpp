#include "FPSCounter.h"

namespace BHive
{
	void FPSCounter::Begin()
	{
		mTimer = Timer();

		mDeltaTime += Time::GetDeltaTime();
		if (mDeltaTime > 1.0)
		{
			mDeltaTime = 0.f;
			mFrames = 0;
		}
	}

	void FPSCounter::End()
	{
		mFrames++;
	}
} // namespace BHive
