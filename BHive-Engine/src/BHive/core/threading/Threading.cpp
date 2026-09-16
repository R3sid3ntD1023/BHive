#include "Threading.h"
#include <queue>

namespace BHive
{
	void Thread::Worker()
	{

		while (sRunning.load())
		{
			Job job;

			{
				std::unique_lock lock(sMutex);

				sCV.wait(lock, []() { return !sRunning.load() || !sQueue.Empty(); });

				if (!sRunning.load())
					return;

				sQueue.Pop(job);
			}

			job.Work();

			sStates[job.Handle.Index].UnFinishedJobs.fetch_sub(1);
		}
	}

	JobHandle Thread::AllocateHandle()
	{
		static std::atomic<uint16_t> next = 0;
		uint16_t index = next.fetch_add(1) % MAX_JOBS;

		++sStates[index].Generation;

		return {index, sStates[index].Generation};
	}

	bool Thread::IsDone(JobHandle handle)
	{
		const JobState &state = sStates[handle.Index];

		if (state.Generation != handle.Generation)
			return true;

		return state.UnFinishedJobs.load() == 0;
	}

	void Thread::Init()
	{
		sRunning.store(true);

		for (auto &worker : sWorkers)
		{
			worker = std::thread(Thread::Worker);
		}
	}

	void Thread::Shutdown()
	{
		sRunning.store(false);

		sCV.notify_all();

		for (auto &worker : sWorkers)
		{
			if (worker.joinable())
				worker.join();
		}
	}

	JobHandle Thread::Schedule(JobFunction func)
	{
		const JobHandle handle = AllocateHandle();

		sStates[handle.Index].UnFinishedJobs.store(1);

		Job job{.Work = std::move(func), .Handle = handle};

		{
			std::lock_guard lock(sMutex);

			ASSERT(sQueue.Push(std::move(job)), "Job Queue Full");
		}

		sCV.notify_one();

		return handle;
	}

	void Thread::Wait(JobHandle handle)
	{
		while (!IsDone(handle))
		{
			Job job;

			{
				{
					std::lock_guard lock(sMutex);

					if (!sQueue.Pop(job))
					{
						std::this_thread::yield();
						continue;
					}
				}

				job.Work();

				sStates[job.Handle.Index].UnFinishedJobs.fetch_sub(1);
			}
		}
	}

	void Thread::ParallelFor(uint32_t count, uint32_t batchSize, ParallelJobFunction &&func)
	{
		if (count == 0)
			return;

		if (count <= batchSize)
		{
			for (uint32_t i = 0; i < count; ++i)
			{
				func(i);
			}

			return;
		}

		const uint32_t batchCount = (count + batchSize - 1) / batchSize;
		const JobHandle parent = AllocateHandle();

		sStates[parent.Index].UnFinishedJobs.store(batchCount);

		for (uint32_t batch = 0; batch < batchCount; ++batch)
		{
			Schedule(
				[=]()
				{
					uint32_t begin = batch * batchSize;
					uint32_t end = std::min(begin + batchSize, count);
					for (uint32_t i = begin; i < end; ++i)
					{
						func(i);
					}

					sStates[parent.Index].UnFinishedJobs.fetch_sub(1);
				});
		}

		Wait(parent);
	}

} // namespace BHive