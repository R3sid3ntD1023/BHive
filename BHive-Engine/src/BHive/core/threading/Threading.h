#pragma once

#include "core/Core.h"

namespace BHive
{
	constexpr uint32_t MAX_WORKERS = 8;
	constexpr uint32_t MAX_JOBS = 4096;
	constexpr uint32_t QUEUE_SIZE = 8192;

	using JobFunction = std::function<void()>;
	using ParallelJobFunction = std::function<void(uint32_t)>;

	struct JobHandle
	{
		static constexpr uint16_t InvalidIndex = UINT16_MAX;

		uint16_t Index = InvalidIndex;
		uint16_t Generation = 0;

		bool IsValid() const { return Index != InvalidIndex; }
	};

	struct Job
	{
		JobFunction Work;
		JobHandle Handle;
	};

	struct JobState
	{
		std::atomic<uint32_t> UnFinishedJobs = 0;
		uint16_t Generation = 0;
	};

	template <typename T, size_t Capacity>
	class RingBuffer
	{
	public:
		bool Push(T &&v)
		{
			uint32_t nextTail = (mTail + 1) % Capacity;
			if (nextTail == mHead)
				return false;

			mData[mTail] = std::move(v);
			mTail = nextTail;

			return true;
		}

		bool Pop(T &out)
		{
			if (mHead == mTail)
				return false;

			out = std::move(mData[mHead]);

			mHead = (mHead + 1) % Capacity;

			return true;
		}

		bool Empty() const { return mHead == mTail; }

	private:
		std::array<T, Capacity> mData;
		uint32_t mHead = 0;
		uint32_t mTail = 0;
	};

	class BHIVE_API Thread
	{
	public:
		static void Init();

		static void Shutdown();

		static JobHandle Schedule(JobFunction func);

		static void Wait(JobHandle handle);

		static void ParallelFor(uint32_t count, uint32_t batchSize, ParallelJobFunction &&func);

		static bool IsDone(JobHandle handle);

	private:
		static void Worker();

		static JobHandle AllocateHandle();

	private:
		inline static RingBuffer<Job, QUEUE_SIZE> sQueue;

		inline static std::mutex sMutex;

		inline static std::condition_variable sCV;

		inline static std::atomic<bool> sRunning = false;

		inline static std::array<std::thread, MAX_WORKERS> sWorkers;

		inline static std::array<JobState, MAX_JOBS> sStates;
	};

#define BEGIN_THREAD_DISPATCH(...) Thread::Schedule([__VA_ARGS__](){
#define END_THREAD_DISPATCH() \
	});
} // namespace BHive