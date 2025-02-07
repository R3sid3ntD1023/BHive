#pragma once

#include "math/Math.h"
#include <cereal/cereal.hpp>
#include <string>

namespace glm
{
	/*template <typename A, length_t L, typename T, qualifier Q>
	inline void CEREAL_SERIALIZE_FUNCTION_NAME(A &ar, vec<L, T, Q> &obj)
	{
		ar(cereal::binary_data(&obj, sizeof(vec<L, T, Q>)));
	}*/

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

	template <typename A, length_t L, typename T, qualifier Q>
	inline std::string CEREAL_SAVE_MINIMAL_FUNCTION_NAME(A &ar, const vec<L, T, Q> &obj)
	{
		std::stringstream ss;
		ss << "{";
		for (int i = 0; i < L; i++)
		{
			ss << obj[i];
			if (i < L - 1)
				ss << ",";
		}
		ss << "}";
		return ss.str();
	}

	template <typename A, length_t L, typename T, qualifier Q>
	inline void CEREAL_LOAD_MINIMAL_FUNCTION_NAME(A &ar, vec<L, T, Q> &obj, const std::string &v)
	{
		char token;
		std::stringstream ss(v);
		ss >> token;

		for (int i = 0; i < L; i++)
		{
			ss >> obj[i];
			if (i < L - 1)
				ss >> token;
		}
		ss >> token;
	}

} // namespace glm