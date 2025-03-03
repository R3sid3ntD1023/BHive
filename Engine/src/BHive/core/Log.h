#pragma once

#include "core/Core.h"
#include "core/EventDelegate.h"

#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

namespace BHive
{
	class Log
	{
	public:
		static BHIVE void Init();

		static BHIVE Ref<spdlog::logger> &GetLogger() { return sLogger; }

		static inline EventDelegate<const spdlog::details::log_msg &> OnMessageLogged;

	private:
		static inline Ref<spdlog::logger> sLogger;
	};
} // namespace BHive

template <>
struct fmt::formatter<std::filesystem::path> : fmt::formatter<std::string>
{
	using formatted_type = std::filesystem::path;

	template <typename ParseContext>
	constexpr auto parse(ParseContext &ctx)
	{
		return ctx.begin();
	}

	template <typename FormatContext>
	auto format(const formatted_type &v, FormatContext &ctx) const
	{
		return fmt::format_to(ctx.out(), "{}", v.string());
	}
};

#if _DEBUG
	#define LOG_INFO(...) ::BHive::Log::GetLogger()->info(__VA_ARGS__);
	#define LOG_TRACE(...) ::BHive::Log::GetLogger()->trace(__VA_ARGS__);
	#define LOG_WARN(...) ::BHive::Log::GetLogger()->warn(__VA_ARGS__);
	#define LOG_ERROR(...) ::BHive::Log::GetLogger()->error(__VA_ARGS__);
	#define LOG_CRITICAL(...) ::BHive::Log::GetLogger()->critical(__VA_ARGS__);
#else
	#define LOG_INFO(...)
	#define LOG_TRACE(...)
	#define LOG_WARN(...)
	#define LOG_ERROR(...)
	#define LOG_CRITICAL(...)

#endif