#pragma once

#include <tuple>
#include <type_traits>

namespace BHive
{

	namespace traits
	{

		template <typename... T>
		struct args
		{

			static constexpr size_t NumArgs = sizeof...(T);

			template <size_t Index>
			using Args = std::tuple_element_t<Index, std::tuple<T...>>;
		};

		template <typename R, typename... T>
		struct function_signiture
		{
			using signiture = R (*)(T...);
			using args_t = args<T...>;
		};

		template <typename... T>
		struct has_args : std::true_type
		{
			static constexpr size_t NumArgs = sizeof...(T);

			template <size_t Index>
			using Args = std::tuple_element_t<Index, std::tuple<T...>>;
		};

		template <>
		struct has_args<> : std::false_type
		{
		};

		template <typename... T>
		using has_args_t = typename std::enable_if<has_args<T...>::value>::type;

		template <typename... T>
		using has_no_args_t = typename std::enable_if<!has_args<T...>::value>::type;
	}
}