#pragma once

#include "core/Core.h"

namespace BHive
{
	class Query
	{
	public:
		Query();
		virtual ~Query();

		virtual void Begin();
		virtual void End();

		virtual double GetTime() const { return mInstanceTime; }
		virtual double GetMinTime() const { return mMinTime; }
		virtual double GetMaxTime() const { return mMaxTime; }
		virtual double GetTimeSum() const { return mTimeAccumulated; }

	private:
		double mTimeAccumulated = 0;
		double mMinTime = 0;
		double mMaxTime = 0;
		double mInstanceTime = 0;
		uint32_t mQueryIDs[2];
	};

} // namespace BHive
