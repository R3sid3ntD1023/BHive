#pragma once

#include "ReflectionCore.h"
#include "PropertyMetaData.h"
#include "Formatters.h"

// policies
#define CONSTRUCTOR_POLICY_OBJECT (rttr::policy::ctor::as_object)
#define CONSTRUCTOR_POLICY_SHARED (rttr::policy::ctor::as_std_shared_ptr)
#define CONSTRUCTOR_POLICY_PTR (rttr::policy::ctor::as_raw_ptr)
#define PROPERTY_POLICY_AS_PTR rttr::policy::prop::bind_as_ptr
#define PROPERTY_POLICY_AS_REFERENCE_WRAPPER (rttr::policy::prop::as_reference_wrapper)

#define REFLECT_PROPERTY_GETTER_SETTER_IMPL(name, getter, setter) .property(name, &T::##getter, &T::##setter)
#define REFLECT_PROPERTY_IMPL(name, member) .property(name, &T::##member)
#define REFECT_PROPERTY_NO_NAME_IMPL(member) REFLECT_PROPERTY_IMPL(#member, member)

#define GET_REFLECT_PROPERTY_MACRO_NAME(arg0, arg1, arg2, macro) macro
#define GET_REFLECT_PROPERTY_MACRO(...) EXPAND(GET_REFLECT_PROPERTY_MACRO_NAME(__VA_ARGS__, REFLECT_PROPERTY_GETTER_SETTER_IMPL, REFLECT_PROPERTY_IMPL, REFECT_PROPERTY_NO_NAME_IMPL))

#define REFLECT_PROPERTY(...) EXPAND(GET_REFLECT_PROPERTY_MACRO(__VA_ARGS__)(__VA_ARGS__))
#define REFLECT_PROPERTY_READ_ONLY(name, member) .property_readonly(name, &T::##member)

#define REFLECT_CONSTRUCTOR(...) .constructor<__VA_ARGS__>()
#define REFLECT_METHOD(name, func) .method(name, func)

#define SELECT_OVERLOAD(function, ...) rttr::select_overload<__VA_ARGS__>(&T::##function)

#define AUTO_REFLECTION_FUNCTION_NAME rttr_auto_register_reflection_function_t
#define REFLECT_IMPLEMTATION(cls) static const auto ANONYMOUS_VARIABLE(auto_register__) = reflect_detail::refl(&AUTO_REFLECTION_FUNCTION_NAME<cls>);
#define REFLECT_INLINE(cls)                              \
	template <typename T>                                \
	extern void AUTO_REFLECTION_FUNCTION_NAME();         \
	template <>                                          \
	void BHIVE_API AUTO_REFLECTION_FUNCTION_NAME<cls>(); \
	REFLECT_IMPLEMTATION(cls)                            \
	template <>                                          \
	inline void AUTO_REFLECTION_FUNCTION_NAME<cls>()

#define REFLECT(cls) \
	template <>      \
	void AUTO_REFLECTION_FUNCTION_NAME<cls>()

#define REFLECT_EXTERN(cls)                              \
	template <typename T>                                \
	extern void AUTO_REFLECTION_FUNCTION_NAME();         \
	template <>                                          \
	void BHIVE_API AUTO_REFLECTION_FUNCTION_NAME<cls>(); \
	REFLECT_IMPLEMTATION(cls)

#define REFLECTABLE_FRIEND()                     \
	template <typename T>                        \
	friend void AUTO_REFLECTION_FUNCTION_NAME(); \
	RTTR_REGISTRATION_FRIEND

#define REFLECTABLE() REFLECTABLE_FRIEND()
#define REFLECTABLE_CLASS(...) REFLECTABLE_FRIEND() RTTR_ENABLE(__VA_ARGS__)
#define REFLECTABLEV(...) REFLECTABLE_FRIEND() RTTR_ENABLE(__VA_ARGS__)

#define BEGIN_REFLECT_IMPL(cls, name) \
	using T = cls;                    \
	auto factory = rttr::registration::class_<cls>(name)

#define BEGIN_REFLECT_WITH_NAME(cls, name) BEGIN_REFLECT_IMPL(cls, name)
#define BEGIN_REFLECT_NO_NAME(cls) BEGIN_REFLECT_IMPL(cls, #cls)
#define BEGIN_REFLECT_GET_MACRO(arg0, arg1, macro) macro
#define BEGIN_REFLECT_GET_MACRO_NAME(...) EXPAND(BEGIN_REFLECT_GET_MACRO(__VA_ARGS__, BEGIN_REFLECT_WITH_NAME, BEGIN_REFLECT_NO_NAME))
#define BEGIN_REFLECT(...) EXPAND(BEGIN_REFLECT_GET_MACRO_NAME(__VA_ARGS__)(__VA_ARGS__))

#define BEGIN_REFLECT_ENUM(cls) \
	using TEnum = cls;          \
	rttr::registration::enumeration<cls>(#cls)

#define GET_ENUM_VALUE_MACRO_NAME(arg0, macro) macro

#define ENUM_VALUE_IMPL_1(enum_value, name) rttr::value(name, TEnum::##enum_value)

#define ENUM_VALUE_IMPL_2(enum_value) ENUM_VALUE_IMPL_1(enum_value, #enum_value)

#define GET_ENUM_VALUE_MACRO(enum_value, ...) EXPAND(GET_ENUM_VALUE_MACRO_NAME(__VA_ARGS__, ENUM_VALUE_IMPL_2, ENUM_VALUE_IMPL_1))
#define ENUM_VALUE(...) EXPAND(GET_ENUM_VALUE_MACRO(__VA_ARGS__)(__VA_ARGS__))

#define META_DATA(key, value) rttr::metadata(key, value)

#define DECLARE_CLASS(...)
#define DECLARE_STRUCT(...)
#define DECLARE_CONSTRUCTOR(...)
#define DECLARE_PROPERTY(...)
#define DECLARE_FUNCTION(...)
#define DECLARE_ENUM(...)

namespace BHive
{
	static const rttr::type InvalidType = rttr::type::get<rttr::detail::invalid_type>();
}

namespace reflect_detail
{
	struct BHIVE_API refl
	{
		refl(void (*f)()) { f(); };
	};

} // namespace reflect_detail
