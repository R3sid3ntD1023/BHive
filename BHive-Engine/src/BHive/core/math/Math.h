#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_SWIZZLE
#define PI glm::pi<float>()

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fmt/format.h>

template <glm::length_t L, typename T, glm::qualifier Q>
struct fmt::formatter<glm::vec<L, T, Q>> : fmt::formatter<std::string>
{
	using formatted_type = glm::vec<L, T, Q>;

	template <typename ParseContext>
	constexpr auto parse(ParseContext &ctx)
	{
		return ctx.begin();
	}

	template <typename FormatContext>
	auto format(const formatted_type &v, FormatContext &ctx) const
	{
		return fmt::format_to(ctx.out(), "{}", glm::to_string(v));
	}
};

namespace glm
{
	template <typename Ostream, length_t L, typename T, qualifier Q>
	inline Ostream &operator<<(Ostream &os, const vec<L, T, Q> &vec)
	{
		os << '(';
		for (length_t i{}; i < L; i++)
		{
			os << vec[i];
			if (i < L - 1)
				os << ',';
		}

		os << ')';
		return os;
	}

	template <typename Istream, length_t L, typename T, qualifier Q>
	inline Istream &operator>>(Istream &is, vec<L, T, Q> &vec)
	{
		char token;
		is >> token;
		for (length_t i{}; i < L; i++)
			is >> vec[i] >> token;

		return is;
	}

} // namespace glm