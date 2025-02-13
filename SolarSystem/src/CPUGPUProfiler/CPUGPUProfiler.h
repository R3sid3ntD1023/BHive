#pragma once

#include "core/Core.h"

#define MAX_PROFILE_SAMPLES 100

namespace BHive
{
	class QueryTimer;

	struct ProfilerData
	{
		using Samples = std::array<float, MAX_PROFILE_SAMPLES>;

		void AddSample(float sample);

		const Samples &GetSamples() const { return mSamples; }
		float GetCurrentSample() const { return mSamples[mCurrentSample]; }
		float GetMinSample() const { return mMinSample; }
		float GetMaxSample() const { return mMaxSample; }

	private:
		Samples mSamples;
		size_t mCurrentSample = 0;
		float mMinSample = 0;
		float mMaxSample = 0;
	};

	struct CPUGPUProfiler
	{
		using GPUData = std::unordered_map<std::string, ProfilerData>;

		static CPUGPUProfiler &GetInstance();

		const GPUData &GetData() const { return mData; }

		void AddData(const std::string &name, float time);
		Ref<QueryTimer> GetQuery(const std::string &name);

	private:
		std::unordered_map<std::string, Ref<QueryTimer>> mQueries;
		GPUData mData;
	};

	struct ScopedProfiler
	{
		ScopedProfiler(const char *name)
			: mName(name)
		{
		}

		const char *mName;
	};

	struct ScopedGPUProfiler : public ScopedProfiler
	{
		ScopedGPUProfiler(const char *name);
		~ScopedGPUProfiler();

	private:
		Ref<QueryTimer> mQueryInstance;
	};

	struct ScopedCPUProfiler : public ScopedProfiler
	{
		ScopedCPUProfiler(const char *name);
		~ScopedCPUProfiler();

	private:
		std::chrono::steady_clock::time_point mStartTimePoint;
	};

	namespace utils
	{
		template <size_t N>
		struct ChangeResult
		{
			char Data[N];
		};

		template <size_t N, size_t K>
		constexpr auto CleanupString(const char (&expr)[N], const char (&remove)[K])
		{
			ChangeResult<N> result = {};

			size_t srcIndex = 0;
			size_t dstIndex = 0;
			while (srcIndex < N)
			{
				size_t matchIndex = 0;
				while (matchIndex < K - 1 && srcIndex + matchIndex < N - 1 && expr[srcIndex + matchIndex] == remove[matchIndex])
					matchIndex++;

				if (matchIndex == K - 1)
					srcIndex += matchIndex;

				result.Data[dstIndex++] = expr[srcIndex] == '"' ? '\'' : expr[srcIndex];
				srcIndex++;
			}

			return result;
		}
	} // namespace utils
} // namespace BHive

#if _DEBUG
	#define ENABLE_PROFILE 1
#endif

#if ENABLE_PROFILE
	#if (defined(__FUNCSIG__) || (_MSC_VER))
		#define PROFILER_FUNC_SIG __FUNCSIG__
	#endif

	#define GPU_PROFILER_SCOPED_IMPL(name, line)                                          \
		constexpr auto fixedname##line = ::BHive::utils::CleanupString(name, "__cdecl "); \
		::BHive::ScopedGPUProfiler profiler##line(fixedname##line.Data)

	#define CPU_PROFILER_SCOPED_IMPL(name, line)                                          \
		constexpr auto fixedname##line = ::BHive::utils::CleanupString(name, "__cdecl "); \
		::BHive::ScopedCPUProfiler profiler##line(fixedname##line.Data)

	#define CPU_PROFILER_SCOPED_LINE(name, line) CPU_PROFILER_SCOPED_IMPL(name, line)
	#define GPU_PROFILER_SCOPED_LINE(name, line) GPU_PROFILER_SCOPED_IMPL(name, line)

	#define GPU_PROFILER_SCOPED(name) GPU_PROFILER_SCOPED_LINE(name, __LINE__)
	#define CPU_PROFILER_SCOPED(name) CPU_PROFILER_SCOPED_LINE(name, __LINE__)

	#define GPU_PROFILER_FUNCTION() GPU_PROFILER_SCOPED(PROFILER_FUNC_SIG)
	#define CPU_PROFILER_FUNCTION() CPU_PROFILER_SCOPED(PROFILER_FUNC_SIG)
#else
	#define GPU_PROFILER_SCOPED(name)
	#define CPU_PROFILER_SCOPED(name)
	#define GPU_PROFILER_FUNCTION()
	#define CPU_PROFILER_FUNCTION()
#endif