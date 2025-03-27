#pragma once

#include <memory>
#include <vector>
#include <any>
#include <array>
#include <queue>
#include <stdint.h>
#include <filesystem>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <iostream>
#include <string>
#include <sstream>
#include <xhash>
#include <future>
#include <thread>
#include <fstream>
#include <type_traits>
#include <mutex>
#include "Version.h"

#ifdef _WIN32
	#ifdef BUILD_DLL
		#ifdef BHIVE_DLL
			#define BHIVE __declspec(dllexport)
		#else
			#define BHIVE __declspec(dllimport)
		#endif
	#else
		#define BHIVE
	#endif
#endif

#define CAT_IMPL_(a, b) a##b
#define CAT_(a, b) CAT_IMPL_(a, b)
#ifdef __COUNTER__
	#define ANONYMOUS_VARIABLE(str) CAT_(str, __COUNTER__)
#else
	#define ANONYMOUS_VARIABLE(str) CAT_(str, __LINE__)
#endif

#define BIT(x) 1 << x
#define EXPAND(x) x
#define STRINGIFY(x) #x
#define COMMA ,
#define BEGIN_NAMESPACE(x) \
	namespace x            \
	{
#define END_NAMESPACE }

#define DECLARE_CLASS(...)
#define DECLARE_STRUCT(...)
#define DECLARE_PROPERTY(...)
#define DECLARE_FUNCTION(...)
#define DECLARE_ENUM(...)

template <typename T>
using Ref = std::shared_ptr<T>;

template <typename T, typename Dx = std::default_delete<T>>
using Scope = std::unique_ptr<T, Dx>;

template <typename T, typename... TArgs>
Ref<T> CreateRef(TArgs &&...args)
{
	return std::make_shared<T>(std::forward<TArgs>(args)...);
}

template <typename T, typename... TArgs>
Scope<T> CreateScope(TArgs &&...args)
{
	return std::make_unique<T>(std::forward<TArgs>(args)...);
}

#include "Casting.h"
#include "Log.h"

#ifdef _DEBUG
	#define ASSERT_IMPL(expression, message, ...) \
		if (!(expression))                        \
		{                                         \
			LOG_ERROR(message, __VA_ARGS__)       \
			__debugbreak();                       \
		}
	#define ASSERT_IMPL_MESSAGE(expression, ...) ASSERT_IMPL(expression, "Assertion at {0}", __VA_ARGS__)
	#define ASSERT_IMPL_NO_MESSAGE(expression) \
		ASSERT_IMPL(expression, "Assertion '{0}' failed at {1}:{2}", STRINGIFY(expression), __FILE__, __LINE__)

	#define ASSERT_GET_MACRO_NAME(arg0, arg1, macro, ...) macro
	#define ASSERT_GET_MACRO(...) EXPAND(ASSERT_GET_MACRO_NAME(__VA_ARGS__, ASSERT_IMPL_MESSAGE, ASSERT_IMPL_NO_MESSAGE))
	#define ASSERT(...) EXPAND(ASSERT_GET_MACRO(__VA_ARGS__)(__VA_ARGS__))
#else
	#define ASSERT(...)
#endif

namespace BHive
{
	static const WAVE::Version sEngineVersion{1, 0, 0};
}

#define GET_ENGINE_VERSION ::BHive::sEngineVersion