#pragma once

#include "core/Core.h"
#include "core/Time.h"

namespace BHive
{
	struct FPSCounter
	{
		void Begin();

		void End();

		operator float() const { return mFrames / mDeltaTime; }

	private:
		float mDeltaTime;
		uint32_t mFrames;
		Timer mTimer;
	};
} // namespace BHive