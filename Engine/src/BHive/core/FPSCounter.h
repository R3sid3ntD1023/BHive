#pragma once

#include "core/Core.h"
#include "core/Time.h"

namespace BHive
{
	struct FPSCounter
	{
		FPSCounter();

		void Frame();

		operator float() const;

	private:
		uint32_t mFrameCount = 0;
		std::chrono::steady_clock::time_point mLastTime;
		double mFPS = 0;
	};
} // namespace BHive