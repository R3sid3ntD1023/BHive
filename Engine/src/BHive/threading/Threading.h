#pragma once

#include <thread>
#include <functional>
#include <queue>
#include <mutex>

namespace BHive
{
	using ThreadFunction = std::function<void()>;

	class IJob
	{
	public:
		virtual ~IJob() = default;
		virtual bool IsDone() = 0;
		virtual ThreadFunction GetDispatchedFunction() = 0;
	};

	class Thread
	{
	public:
		Thread() = default;

		static void PushJob(IJob *job);
		static void Dispatch(ThreadFunction func);
		static void Update();

	private:
		static inline std::vector<IJob *> sJobs;
		static inline std::vector<ThreadFunction> sDispatched;

		using auto_lock = std::lock_guard<std::mutex>;
		static inline std::mutex sMutex;

		static void DispatchInternal(ThreadFunction func);
	};

#define BEGIN_THREAD_DISPATCH(...) Thread::Dispatch([__VA_ARGS__](){
#define END_THREAD_DISPATCH() \
	});
}