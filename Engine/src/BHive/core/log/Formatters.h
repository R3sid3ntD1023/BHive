#pragma once

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
