#pragma once

#include "CoreAPI.h"

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

#include "Config.h"
#include "log/Log.h"
#include "AssertHandler.h"
#include "Casting.h"
#include "math/Math.h"
#include "reflection/Reflection.h"
#include "serialization/Serialization.h"
#include "core/subsystem/SubSystem.h"