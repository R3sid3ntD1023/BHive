#pragma once

#include "core/Log.h"

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <string>
#include <thread>
#include <mutex>
#include <sstream>

namespace BHive
{
	using FloatingPointMicroSeconds = std::chrono::duration<double, std::micro>;

	struct ProfileResult
	{
		std::string Name;

		FloatingPointMicroSeconds Start;
		std::chrono::microseconds ElapsedTime;
		std::thread::id ThreadID;
	};

	struct InstrumentationSession
	{
		std::string Name;
	};

	class Instrumentor
	{
	public:
		Instrumentor(const Instrumentor &) = delete;
		Instrumentor(Instrumentor &&) = delete;

		void BeginSession(const std::string &name, const std::filesystem::path &path = "results.json")
		{
			std::lock_guard lock(mMutex);

			if (mCurrentSession)
			{
				if (Log::GetLogger())
				{
					LOG_ERROR(
						"Instrumentor::BeginSession('{0}') when session '{1}' already open.", name, mCurrentSession->Name);
				}

				InternalEndSession();
			}

			mOutputStream.open(path);

			if (mOutputStream.is_open())
			{
				mCurrentSession = new InstrumentationSession({name});
				WriteHeader();
			}
			else
			{
				if (Log::GetLogger())
				{
					LOG_ERROR("Instrumentor could not open results file '{0}'", path.string());
				}
			}
		}

		void EndSession()
		{
			std::lock_guard lock(mMutex);
			InternalEndSession();
		}

		void WriteProfile(const ProfileResult &result)
		{
			std::stringstream json;

			json << std::setprecision(5) << std::fixed;
			json << ",{";
			json << "\"cat\":\"function\",";
			json << "\"dur\":" << (result.ElapsedTime.count()) << ',';
			json << "\"name\":\"" << result.Name << "\",";
			json << "\"ph\":\"X\",";
			json << "\"pid\":0,";
			json << "\"tid\":" << result.ThreadID << ",";
			json << "\"ts\":" << result.Start.count();
			json << "}";

			std::lock_guard lock(mMutex);
			if (mCurrentSession)
			{
				mOutputStream << json.str();
				mOutputStream.flush();
			}
		}

		static Instrumentor &Get()
		{
			static Instrumentor instance;
			return instance;
		}

	private:
		Instrumentor()
			: mCurrentSession(nullptr)
		{
		}

		~Instrumentor() { EndSession(); }

		void WriteHeader()
		{
			mOutputStream << "{\"otherData\": {},\"traceEvents\":[{}";
			mOutputStream.flush();
		}

		void WriteFooter()
		{
			mOutputStream << "]}";
			mOutputStream.flush();
		}

		void InternalEndSession()
		{
			if (mCurrentSession)
			{
				WriteFooter();
				mOutputStream.close();
				delete mCurrentSession;
				mCurrentSession = nullptr;
			}
		}

	private:
		std::mutex mMutex;
		InstrumentationSession *mCurrentSession = nullptr;
		std::ofstream mOutputStream;
	};

	class InstrumentationTimer
	{
	public:
		InstrumentationTimer(const char *name)
			: mName(name),
			  mStopped(false)
		{
			mStartTimePoint = std::chrono::steady_clock::now();
		}

		~InstrumentationTimer()
		{
			if (!mStopped)
				Stop();
		}

		void Stop()
		{
			auto end_point = std::chrono::steady_clock::now();
			auto high_res_start = FloatingPointMicroSeconds{mStartTimePoint.time_since_epoch()};
			auto elaped_time = std::chrono::time_point_cast<std::chrono::microseconds>(end_point).time_since_epoch() -
							   std::chrono::time_point_cast<std::chrono::microseconds>(mStartTimePoint).time_since_epoch();

			Instrumentor::Get().WriteProfile({mName, high_res_start, elaped_time, std::this_thread::get_id()});

			mStopped = true;
		}

	private:
		const char *mName;
		std::chrono::time_point<std::chrono::steady_clock> mStartTimePoint;
		bool mStopped;
	};

	namespace InstrumentorUtils
	{
		template <size_t N>
		struct ChangeResult
		{
			char Data[N];
		};

		template <size_t N, size_t K>
		constexpr auto CleanupOutputString(const char (&expr)[N], const char (&remove)[K])
		{
			ChangeResult<N> result = {};

			size_t srcIndex = 0;
			size_t dstIndex = 0;
			while (srcIndex < N)
			{
				size_t matchIndex = 0;
				while (matchIndex < K - 1 && srcIndex + matchIndex < N - 1 &&
					   expr[srcIndex + matchIndex] == remove[matchIndex])
					matchIndex++;

				if (matchIndex == K - 1)
					srcIndex += matchIndex;

				result.Data[dstIndex++] = expr[srcIndex] == '"' ? '\'' : expr[srcIndex];
				srcIndex++;
			}

			return result;
		}
	} // namespace InstrumentorUtils
} // namespace BHive

#if _DEBUG
	#define BH_PROFILE 1
#endif

#if BH_PROFILE
	#if (defined(__FUNCSIG__) || (_MSC_VER))
		#define BH_FUNC_SIG __FUNCSIG__
	#endif
	#define BH_PROFILE_BEGIN_SESSION(name, filepath) ::BHive::Instrumentor::Get().BeginSession(name, filepath)
	#define BH_PROFILE_END_SESSION() ::BHive::Instrumentor::Get().EndSession()
	#define BH_PROFILE_SCOPE_LINE2(name, line)                                                             \
		constexpr auto fixedName##line = ::BHive::InstrumentorUtils::CleanupOutputString(name, "__cdel "); \
		::BHive::InstrumentationTimer timer##line(fixedName##line.Data)

	#define BH_PROFILE_SCOPE_LINE(name, line) BH_PROFILE_SCOPE_LINE2(name, line)
	#define BH_PROFILE_SCOPE(name) BH_PROFILE_SCOPE_LINE(name, __LINE__)
	#define BH_PROFILE_FUNCTION() BH_PROFILE_SCOPE(BH_FUNC_SIG)
#else
	#define BH_PROFILE_BEGIN_SESSION(name, filepath)
	#define BH_PROFILE_END_SESSION()
	#define BH_PROFILE_SCOPE(name)
	#define BH_PROFILE_FUNCTION()
#endif