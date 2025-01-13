#pragma once

#include <cereal/cereal.hpp>
#include "math/Math.h"
#include <string>


namespace glm
{
	template <typename A, length_t L, typename T, qualifier Q>
	inline std::string CEREAL_SAVE_MINIMAL_FUNCTION_NAME(A &ar, const vec<L, T, Q> &obj)
	{
		std::stringstream ss;
		ss << '{';
		for (size_t i = 0; i < L; i++)
		{
			ss << obj[i];
			if (i < L)
				ss << ',';
		}

		ss << '}';

		return ss.str();
	}

	template <typename A, length_t L, typename T, qualifier Q>
	inline void
	CEREAL_LOAD_MINIMAL_FUNCTION_NAME(A &ar, vec<L, T, Q> &obj, const std::string &value)
	{
		char token;
		::std::stringstream ss(value);
		ss >> token;

		for (size_t i = 0; i < L; i++)
		{
			ss >> obj[i];
			if (i < L)
				ss >> token;
		}

		ss >> token;
	}

	template <typename TArchive, length_t C, length_t R, typename T, qualifier Q>
	inline std::string CEREAL_SAVE_MINIMAL_FUNCTION_NAME(TArchive &ar, const mat<C, R, T, Q> &obj)
	{
		std::stringstream ss;
		ss << '{';
		for (size_t i = 0; i < C; i++)
		{
			for (size_t j = 0; j < R; j++)
			{
				ss << obj[i][j];
				if (j < R)
					ss << ',';
			}

			if (i < C)
				ss << ',';
		}

		ss << '}';

		return ss.str();
	}

	template <typename TArchive, length_t C, length_t R, typename T, qualifier Q>
	inline void
	CEREAL_LOAD_MINIMAL_FUNCTION_NAME(TArchive &ar, mat<C, R, T, Q> &obj, const std::string &value)
	{
		char token;
		std::stringstream ss(value);

		ss >> token;
		for (size_t i = 0; i < C; i++)
		{
			for (size_t j = 0; j < R; j++)
			{
				ss >> obj[i][j];
				if (j < R)
					ss >> token;
			}

			if (i < C)
				ss >> token;
		}
	}

	template <typename A, typename T, qualifier Q>
	inline std::string CEREAL_SAVE_MINIMAL_FUNCTION_NAME(A &ar, const qua<T, Q> &q)
	{
		std::stringstream ss;
		ss << '{' << q.x << ',' << q.y << ',' << q.z << ',' << q.w << '}';
		return ss.str();
	}

	template <typename A, typename T, qualifier Q>
	inline void CEREAL_LOAD_MINIMAL_FUNCTION_NAME(A &ar, qua<T, Q> &q, const std::string &value)
	{
		char token;
		std::stringstream ss(value);
		ss >> token >> q.x >> token >> q.y >> token >> q.z >> token >> q.w >> token;

	} // namespace glm

} // namespace glm