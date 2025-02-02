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
		float mDeltaTime{0.f};
		uint32_t mFrames{0};
		Timer mTimer;
	};
} // namespace BHive