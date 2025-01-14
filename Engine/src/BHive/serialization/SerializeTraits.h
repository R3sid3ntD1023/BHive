#pragma once

#include "Accessor.h"
#include <type_traits>

namespace BHive
{
    namespace details
    {
        using Yes = std::true_type;
        using No = std::false_type;

        struct Any
        {
            template<typename T>
			operator T &();

            template<typename T>
			operator T& const() const;
        };

 #define MAKE_VERSION_TEST ,0
#define MAKE_SERIALIZE_FUNCTION_TRAIT(name, func, versioned)\
        template <typename A, typename T>\
        struct has_##name\
        {\
            template <typename A, typename T>\
            static auto test(int) -> decltype(func(std::declval<A &>(), std::declval<const T&>() versioned), Yes());\
                \
            template <typename, typename>\
            static No test(...);\
                \
            static const bool value = std::is_same_v<decltype(test<A, T>(0)), Yes>;\
        };

#define MAKE_DESERIALIZE_FUNCTION_TRAIT(name, func, versioned)\
        template <typename A, typename T>\
        struct has_##name\
        {\
            template <typename A, typename T>\
            static auto test(int) -> decltype(func(std::declval<A &>(), std::declval<T &>() versioned),             \
										  Yes());\
                \
            template <typename, typename>\
            static No test(...);\
                \
            static const bool value = std::is_same_v<decltype(test<A, T>(0)), Yes>;\
        };


        MAKE_SERIALIZE_FUNCTION_TRAIT(serialize, accessor::Serialize, );
		MAKE_SERIALIZE_FUNCTION_TRAIT(serialize_non_member, Serialize, );
		MAKE_SERIALIZE_FUNCTION_TRAIT(serialize_versioned, accessor::Serialize, MAKE_VERSION_TEST);
		MAKE_DESERIALIZE_FUNCTION_TRAIT(deserialize, accessor::Deserialize, );
		MAKE_DESERIALIZE_FUNCTION_TRAIT(deserialize_non_member, Deserialize, );
		MAKE_DESERIALIZE_FUNCTION_TRAIT(deserialize_versioned, accessor::Deserialize,
										MAKE_VERSION_TEST);
    }

   

    template <typename A, typename T>
    const bool has_serialize_v = details::has_serialize<A,T>::value;

    template <typename A, typename T>
    const bool has_serialize_non_member_v = details::has_serialize_non_member<A, T>::value;

    template <typename A, typename T>
	const bool has_serialized_versioned = details::has_serialize_versioned<A, T>::value;

    template <typename A, typename T>
    const bool has_deserialize_v = details::has_deserialize<A, T>::value;

    template <typename A, typename T>
    const bool has_deserialize_non_member_v = details::has_deserialize_non_member<A, T>::value;

    template <typename A, typename T>
	const bool has_deserialze_versionsed = details::has_deserialize_versioned<A, T>::value;


    template<typename A, typename T>
    struct serialize_counter : std::integral_constant <int, has_serialize_non_member_v<A, T> + has_serialize_v<A, T> +
        has_serialized_versioned<A, T>>
    { };

    template<typename A, typename T>
    struct deserialize_counter : std::integral_constant <int, has_deserialize_non_member_v<A, T> + has_deserialize_v<A, T> +
        has_deserialze_versionsed<A, T> >
    {
    };

    template<typename A, typename T>
    struct is_serializable : std::integral_constant<bool, serialize_counter<A, T>::value == 1> 
    {

    };

    template <typename A, typename T>
    const bool is_serializable_v = is_serializable<A, T>::value;

    template<typename A, typename T>
    struct is_deserializable : std::integral_constant<bool, deserialize_counter<A, T>::value == 1> {};

    template <typename A, typename T>
    const bool is_deserializable_v = is_deserializable<A, T>::value;
} // namespace BHive
