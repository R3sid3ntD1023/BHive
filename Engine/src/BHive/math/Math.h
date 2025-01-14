#pragma once

#include "core/Core.h"
#include "Vectors.h"

#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_SWIZZLE

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "serialization/Serialization.h"

namespace glm
{
	template <typename Ostream, length_t L, typename T, qualifier Q>
	inline Ostream &operator<<(Ostream &os, const vec<L, T, Q> &vec)
	{
		return os << to_string(vec);
	}

}

#define PI 22.f / 7.f

namespace BHive
{
	template <glm::length_t L, typename T, glm::qualifier Q>
	void Serialize(StreamWriter &ar, const glm::vec<L, T, Q> &obj)
	{
		for (size_t i = 0; i < L; i++)
			ar(obj[i]);
	}

	template <glm::length_t L, typename T, glm::qualifier Q>
	void Deserialize(StreamReader &ar, glm::vec<L, T, Q> &obj)
	{
		for (size_t i = 0; i < L; i++)
			ar(obj[i]);
	}
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
	};
}