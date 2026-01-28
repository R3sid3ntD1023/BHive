#pragma once

#include "core/Core.h"

namespace BHive
{
	class BHIVE_API Time
	{
	public:
		static float Get();
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