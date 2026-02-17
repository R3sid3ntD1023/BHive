#pragma once

#include "core/Core.h"

namespace BHive
{
	class BHIVE_API Time
	{
	public:
		static float Raw();

		static float DeltaTime();

		static void Update();

	private:
		static inline float mTime = 0.0f;
		static inline float mDeltaTime = 0.0f;
		static inline float mLastTime = 0.0f;
	};

	class BHIVE_API Timer
	{
	public:
		Timer();

		void Reset();

		float Elasped();

		float ElaspedMillis();

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> mStart;
	};
} // namespace BHive