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
		static T mix(const T &t0, const T &t1, float w);

		template <typename T>
		static T lerp(const T &v0, const T &v1, float t);

		template <typename T>
		static T remap(const T &v, const T &inMin, const T &inMax, const T &outMin, const T &outMax);

		template <typename T>
		static T normalize(const T &v, const T &min, const T &max);

		static glm::vec3 normalize_device_coordinates(float x, float y, float width, float height);

		static glm::vec4 ndc_to_eye_coordinates(const glm::vec3 &ndc_coordinates, const glm::mat4 &projection_matrix);

		static glm::vec3 eye_to_world_coordinates(const glm::vec4 &eye_coordinates, const glm::mat4 &view_matrix);

		static glm::vec3 get_mouse_ray(
			float x, float y, float width, float height, const glm::mat4 &projection_matrix, const glm::mat4 &view_matrix);

		// construct a plane
		static FPlane create_plane(const glm::vec3 &p1, const glm::vec3 &normal);

		static float get_signed_distance_to_plane(const FPlane &plane, const glm::vec3 &point);
	};
} // namespace BHive

#include "MathFunctionLibrary.inl"