#include "CPUGPUProfiler.h"
#include "gfx/Query.h"
#include <glm/glm.hpp>

namespace BHive
{
	CPUGPUProfiler &CPUGPUProfiler::GetInstance()
	{
		static CPUGPUProfiler profiler;
		return profiler;
	}

	void CPUGPUProfiler::AddData(const std::string &name, float time)
	{
		mData[name].AddSample(time);
	}

	Ref<QueryTimer> CPUGPUProfiler::GetQuery(const std::string &name)
	{
		if (!mQueries.contains(name))
			mQueries.emplace(name, QueryTimer::Create());

		return mQueries.at(name);
	}

	void ProfilerData::AddSample(float sample)
	{
		mSamples[mCurrentSample++] = sample;
		mCurrentSample = mCurrentSample % MAX_PROFILE_SAMPLES;

		mMinSample = glm::min(mMinSample, sample);
		mMaxSample = glm::max(mMaxSample, sample);
	}

	ScopedGPUProfiler::ScopedGPUProfiler(const char *name)
		: ScopedProfiler(name)
	{
		mQueryInstance = CPUGPUProfiler::GetInstance().GetQuery(name);
		mQueryInstance->Begin();
	}

	ScopedGPUProfiler::~ScopedGPUProfiler()
	{
		mQueryInstance->End();

		// seconds
		auto time = mQueryInstance->GetTime() / 1000.f;
		CPUGPUProfiler::GetInstance().AddData(mName, time);
	}

	ScopedCPUProfiler::ScopedCPUProfiler(const char *name)
		: ScopedProfiler(name)
	{
		mStartTimePoint = std::chrono::high_resolution_clock::now();
	}

	ScopedCPUProfiler::~ScopedCPUProfiler()
	{
		auto elasped = std::chrono::high_resolution_clock::now() - mStartTimePoint;

		// seconds
		auto time = std::chrono::duration_cast<std::chrono::milliseconds>(elasped).count() / 1000.f;

		CPUGPUProfiler::GetInstance().AddData(mName, time);
	}
} // namespace BHive