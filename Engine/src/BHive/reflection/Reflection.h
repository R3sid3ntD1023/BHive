#pragma once

#include "core/Core.h"
#include <rttr/registration>
#include <rttr/registration_friend>
#include <rttr/rttr_enable.h>
#include <rttr/type.h>
#include "PropertyMetaData.h"

#define REFLECT_PROPERTY_GETTER_SETTER_IMPL(name, getter, setter) \
	.property(name, &T::##getter, &T::##setter)
#define REFLECT_PROPERTY_IMPL(name, member) .property(name, &T::##member)

#define GET_REFLECT_PROPERTY_MACRO_NAME(arg0, arg1, macro) macro
#define GET_REFLECT_PROPERTY_MACRO(name, ...)                                                \
	EXPAND(GET_REFLECT_PROPERTY_MACRO_NAME(__VA_ARGS__, REFLECT_PROPERTY_GETTER_SETTER_IMPL, \
										   REFLECT_PROPERTY_IMPL))

#define REFLECT_PROPERTY(...) EXPAND(GET_REFLECT_PROPERTY_MACRO(__VA_ARGS__)(__VA_ARGS__))
#define REFLECT_PROPERTY_READ_ONLY(name, member) .property_readonly(name, &T::##member)

#define REFLECT_CONSTRUCTOR(...) .constructor<__VA_ARGS__>()
#define REFLECT_METHOD(name, func) .method(name, func)

#define CONSTRUCTOR_POLICY_OBJECT (rttr::policy::ctor::as_object)
#define CONSTRUCTOR_POLICY_SHARED (rttr::policy::ctor::as_std_shared_ptr)
#define CONSTRUCTOR_POLICY_PTR (rttr::policy::ctor::as_std_raw_ptr)

#define REFLECT_IMPL(cls)                                               \
	template <>                                                         \
	inline void ::BHive::reflection::auto_register_reflection_t<cls>(); \
	static const auto ANONYMOUS_VARIABLE(auto_register__) =             \
		::BHive::reflection::refl(&::BHive::reflection::auto_register_reflection_t<cls>);

#define REFLECT(cls)  \
	REFLECT_IMPL(cls) \
	template <>       \
	inline void ::BHive::reflection::auto_register_reflection_t<cls>()

#define REFLECT_EXTERN(cls) \
	REFLECT_IMPL(cls)       \
	template <>             \
	extern void ::BHive::reflection::auto_register_reflection_t<cls>();

#define REFLECTABLE_FRIEND() \
	template <typename T>    \
	friend void ::BHive::reflection::auto_register_reflection_t<T>();

#define REFLECTABLE() REFLECTABLE_FRIEND()

#define REFLECTABLEV(...) \
	REFLECTABLE_FRIEND()  \
	RTTR_ENABLE(__VA_ARGS__)

#define BEGIN_REFLECT(cls) \
	using T = cls;         \
	auto factory = rttr::registration::class_<cls>(#cls)

#define BEGIN_REFLECT_ENUM(cls) \
	using TEnum = cls;          \
	rttr::registration::enumeration<cls>(#cls)

#define GET_ENUM_VALUE_MACRO_NAME(arg0, macro) macro

#define ENUM_VALUE_IMPL_1(enum_value, name) rttr::value(name, TEnum::##enum_value)

#define ENUM_VALUE_IMPL_2(enum_value) ENUM_VALUE_IMPL_1(enum_value, #enum_value)

#define GET_ENUM_VALUE_MACRO(enum_value, ...) \
	EXPAND(GET_ENUM_VALUE_MACRO_NAME(__VA_ARGS__, ENUM_VALUE_IMPL_2, ENUM_VALUE_IMPL_1))
#define ENUM_VALUE(...) EXPAND(GET_ENUM_VALUE_MACRO(__VA_ARGS__)(__VA_ARGS__))

#define META_DATA(key, value) rttr::metadata(key, value)

namespace BHive
{
	namespace reflection
	{
		template <typename T>
		void auto_register_reflection_t();

		struct refl
		{
			refl(void (*f)()) { f(); }
		};

	} // namespace reflection

} // namespace BHive
