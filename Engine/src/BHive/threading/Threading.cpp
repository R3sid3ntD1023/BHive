#include "Threading.h"
#include <queue>
#include "core/Log.h"

namespace BHive
{
	void Thread::PushJob(IJob *job)
	{
		auto_lock lock(sMutex);
		sJobs.push_back(job);
	}

	void Thread::Dispatch(ThreadFunction func)
	{
		auto_lock lock(sMutex);
		sDispatched.push_back(func);
	}

	void Thread::Update()
	{
		auto_lock lock(sMutex);

		std::queue<IJob *> to_remove;

		for (auto job : sJobs)
		{
			if (job->IsDone())
			{
				auto func = job->GetDispatchedFunction();
				DispatchInternal(func);
				to_remove.push(job);
			}
		}

		for (; !to_remove.empty(); to_remove.pop())
		{
			auto job = to_remove.front();
			sJobs.erase(std::find(sJobs.begin(), sJobs.end(), job));
		}

		for (size_t i = 0; i < sDispatched.size(); i++)
		{
			auto &func = sDispatched[i];

			if (!func)
				continue;

			// LOG_TRACE("{}", func.mName);
			func();
		}

		sDispatched.clear();
	}

	void Thread::DispatchInternal(ThreadFunction func)
	{
		sDispatched.push_back(func);
	}
}