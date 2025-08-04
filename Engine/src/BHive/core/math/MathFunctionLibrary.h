#pragma once

#include "core/Core.h"
#include "Math.h"
#include "Plane.h"

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

namespace BHive
{
	struct BHIVE_API MathFunctionLibrary
	{
		template <typename T>
		static T Mix(const T &t0, const T &t1, float w);

		template <typename T>
		static T Lerp(const T &v0, const T &v1, float t);

		template <typename T>
		static T Remap(const T &v, const T &InMin, const T &InMax, const T &OutMin, const T &OutMax);

		template <typename T>
		static T Normalize(const T &v, const T &min, const T &max);

		static glm::vec3 NormalizeDeviceCoordinates(float x, float y, float width, float height);

		static glm::vec4 NdcToEyeCoordinates(const glm::vec3 &NdcCoordinates, const glm::mat4 &ProjectionMatrix);

		static glm::vec3 EyeToWorldCoordinates(const glm::vec4 &EyeCoordinates, const glm::mat4 &ViewMatrix);

		static glm::vec3 GetMouseRay(float x, float y, float width, float height, const glm::mat4 &ProjectionMatrix, const glm::mat4 &ViewMatrix);

		// construct a plane
		static FPlane CreatePlane(const glm::vec3 &p1, const glm::vec3 &normal);

		static float GetSignedDistanceToPlane(const FPlane &plane, const glm::vec3 &point);
	};
} // namespace BHive

#include "MathFunctionLibrary.inl"