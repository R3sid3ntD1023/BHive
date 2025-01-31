#pragma once

#include "core/Core.h"

namespace BHive
{
	class Time
	{
	public:
		static BHIVE float Get();

		static float GetDeltaTime();
	};

	class Timer
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