#pragma once

#include "log/Log.h"

namespace BHive
{
	inline void AssertHandler(const char* expr, const char* file, int line)
	{
		LOG_ERROR("Assertion Failed: '{}' at {}:{}", expr, file, line);
		__debugbreak();
	}

	template<typename... Args>
	inline void AssertHandler(const char *expr, const char *file, int line, fmt::format_string<Args...> msg, Args&&... args)
	{
		LOG_ERROR("Assertion Failed: '{}' at {}:{} ->", expr, file, line);
		LOG_ERROR(msg, std::forward<Args>(args)...);
		__debugbreak();
	}
}

#ifdef _DEBUG
	#define ASSERT(expr, ...)                                                                    \
		do                                                                                       \
		{                                                                                        \
			if (!(expr))                                                                         \
			{                                                                                    \
				::BHive::AssertHandler(#expr, __FILE__, __LINE__, ##__VA_ARGS__);				 \
			}                                                                                    \
		} while (0);
#else
	#define ASSERT(...)
#endif