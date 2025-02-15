#pragma once

#include "gfx/Query.h"

namespace BHive
{
	class GLQueryTimer : public QueryTimer
	{
	private:
		/* data */
	public:
		GLQueryTimer(/* args */);
		~GLQueryTimer();

		virtual void Begin() override;
		virtual void End() override;
		virtual double GetTime() const override { return mInstanceTime; }
		virtual double GetMinTime() const override { return mMinTime; }
		virtual double GetMaxTime() const override { return mMaxTime; }
		virtual double GetTimeSum() const override { return mTimeAccumulated; }

	private:
		double mTimeAccumulated = 0;
		double mMinTime = 0;
		double mMaxTime = 0;
		double mInstanceTime = 0;
		uint32_t mQueryIDs[2];
	};

} // namespace BHive