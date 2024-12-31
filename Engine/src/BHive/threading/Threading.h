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
		virtual const char *GetName() const = 0;
		virtual ThreadFunction GetDispatchedFunction() = 0;
	};

	struct Function
	{
		std::string mName;
		ThreadFunction mFunc;
	};

	class Thread
	{
	public:
		Thread() = default;

		static void PushJob(IJob *job);
		static void Dispatch(const std::string &name, ThreadFunction func);
		static void Update();

	private:
		static inline std::vector<IJob *> sJobs;
		static inline std::vector<Function> sDispatched;

		using auto_lock = std::lock_guard<std::mutex>;
		static inline std::mutex sMutex;

		static void DispatchInternal(const std::string &name, ThreadFunction func);
	};

#define BEGIN_THREAD_DISPATCH(...) Thread::Dispatch(__FUNCTION__, [__VA_ARGS__](){
#define END_THREAD_DISPATCH() \
	});
}