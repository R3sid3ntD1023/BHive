#pragma once

#include "core/Core.h"
#include "core/Time.h"

namespace BHive
{
	struct BHIVE_API FPSCounter
	{
		FPSCounter();

		void Frame();

		operator float() const;

		static FPSCounter &Get();

	private:
		std::chrono::steady_clock::time_point mLastTime;
		double mFPS = 0;
	};
} // namespace BHive