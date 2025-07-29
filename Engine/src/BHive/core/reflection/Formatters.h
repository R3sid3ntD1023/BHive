#pragma once

#include "ReflectionCore.h"
#include <fmt/format.h>

template <>
struct fmt::formatter<rttr::type> : fmt::formatter<std::string>
{
	template <typename ParseContext>
	constexpr auto parse(ParseContext &ctx)
	{
		return ctx.begin();
	}

	template <typename FormatContext>
	auto format(const rttr::type &v, FormatContext &ctx) const
	{
		return fmt::format_to(ctx.out(), "{}", v.get_name().data());
	}
};

template <>
struct fmt::formatter<rttr::string_view> : fmt::formatter<std::string>
{
	using formatted_type = rttr::string_view;

	template <typename ParseContext>
	constexpr auto parse(ParseContext &ctx)
	{
		return ctx.begin();
	}

	template <typename FormatContext>
	auto format(const formatted_type &v, FormatContext &ctx) const
	{
		return fmt::format_to(ctx.out(), "{}", v.data());
	}
};