#pragma once

#include "core/Core.h"

namespace BHive
{
	class QueryTimer
	{
	public:
		virtual ~QueryTimer() = default;

		virtual void Begin() = 0;
		virtual void End() = 0;
		virtual double GetTime() const = 0;
		virtual double GetMinTime() const = 0;
		virtual double GetMaxTime() const = 0;
		virtual double GetTimeSum() const = 0;

		static Ref<QueryTimer> Create();
	};

} // namespace BHive
