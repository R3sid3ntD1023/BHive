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

template <typename T>
struct fmt::formatter<std::vector<T>> : fmt::formatter<std::string>
{
	using formatted_type = std::vector<T>;

	fmt::formatter<T> elem_formatter;

	template <typename ParseContext>
	constexpr auto parse(ParseContext &ctx)
	{
		return ctx.begin();
	}

	template <typename FormatContext>
	auto format(const formatted_type &v, FormatContext &ctx) const
	{
		auto out = ctx.out();
		*out++ = '[';

		for (size_t i = 0; i < v.size(); i++)
		{
			if (i > 0)
			{
				*out++ = ',';
				*out++ = ' ';
			}

			out = elem_formatter.format(v[i], ctx);
		}

		*out++ = ']';
		return out;
	}
};