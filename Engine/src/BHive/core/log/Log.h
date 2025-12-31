#pragma once

#include "core/CoreAPI.h"

#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

#include <functional>
#include "Formatters.h"

namespace BHive
{
	class BHIVE_API Log
	{
	public:
		static void Init();

		static std::shared_ptr<spdlog::logger> &GetLogger() { return sLogger; }

		static inline std::function<void(const spdlog::details::log_msg &)> OnMessageLogged;

	private:
		static inline std::shared_ptr<spdlog::logger> sLogger;
	};
} // namespace BHive

#if _DEBUG
	#define LOG_INFO(...) ::BHive::Log::GetLogger()->info(__VA_ARGS__);
	#define LOG_TRACE(...) ::BHive::Log::GetLogger()->trace(__VA_ARGS__);
	#define LOG_WARN(...) ::BHive::Log::GetLogger()->warn(__VA_ARGS__);
	#define LOG_ERROR(...)  ::BHive::Log::GetLogger()->error(__VA_ARGS__); 
	#define LOG_CRITICAL(...) ::BHive::Log::GetLogger()->critical(__VA_ARGS__);
#else
	#define LOG_INFO(...)
	#define LOG_TRACE(...)
	#define LOG_WARN(...)
	#define LOG_ERROR(...)
	#define LOG_CRITICAL(...)

#endif