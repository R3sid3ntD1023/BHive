#pragma once

#include "core/Core.h"

#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_SWIZZLE

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>

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

#define PI 22.f / 7.f

namespace BHive
{
	namespace Math
	{
		template <typename T>
		static inline T Lerp(const T &v0, const T &v1, float t)
		{
			return v0 + t * (v1 - v0);
		}

		template <typename T>
		static inline T remap(const T &v, const T &inMin, const T &inMax, const T &outMin, const T &outMax)
		{
			T in_span = inMax - inMin;
			T out_span = outMax - outMin;

			float scaled = (v - inMin) / in_span;

			return outMin + (scaled * out_span);
		}

		template <typename T>
		static inline T normalize(const T &v, const T &min, const T &max)
		{
			return (v - min) / ((max - min));
		}
	}; // namespace Math
} // namespace BHive