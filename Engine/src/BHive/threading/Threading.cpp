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

	void Thread::Dispatch(const std::string &name, ThreadFunction func)
	{
		auto_lock lock(sMutex);
		sDispatched.push_back({name, func});
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
				DispatchInternal(job->GetName(), func);
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

			if (!func.mFunc)
				continue;

			// LOG_TRACE("{}", func.mName);
			func.mFunc();
		}

		sDispatched.clear();
	}

	void Thread::DispatchInternal(const std::string &name, ThreadFunction func)
	{
		sDispatched.push_back({name, func});
	}
}