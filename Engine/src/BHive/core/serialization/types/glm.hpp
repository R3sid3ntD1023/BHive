#pragma once

#include "math/Math.h"
#include <cereal/cereal.hpp>
#include <string>

namespace glm
{
	template <typename A, length_t L, typename T, qualifier Q>
	inline void CEREAL_SERIALIZE_FUNCTION_NAME(A &ar, vec<L, T, Q> &obj)
	{
		ar(cereal::binary_data(&obj, sizeof(vec<L, T, Q>)));
	}

	template <typename TArchive, length_t C, length_t R, typename T, qualifier Q>
	inline void CEREAL_SERIALIZE_FUNCTION_NAME(TArchive &ar, mat<C, R, T, Q> &obj)
	{
		ar(cereal::binary_data(&obj, sizeof(mat<C, R, T, Q>)));
	}

	template <typename A, typename T, qualifier Q>
	inline void CEREAL_SERIALIZE_FUNCTION_NAME(A &ar, qua<T, Q> &obj)
	{
		ar(cereal::binary_data(&obj, sizeof(qua<T, Q>)));
	}

} // namespace glm