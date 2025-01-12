#pragma once

#include <glm/glm.hpp>
#include <string>

namespace glm
{

	template <typename TArchive, length_t L, typename T, qualifier Q>
	void Serialize(TArchive &ar, const vec<L, T, Q> &obj)
	{
		for (size_t i = 0; i < L; i++)
		{
			ar(obj[i]);
		}
	}

	template <typename TArchive, length_t L, typename T, qualifier Q>
	void Deserialize(TArchive &ar, vec<L, T, Q> &obj)
	{
		for (size_t i = 0; i < L; i++)
		{
			ar(obj[i]);
		}

	}

	template <typename TArchive, length_t C, length_t R, typename T, qualifier Q>
	void Serialize(TArchive &ar, const mat<C, R, T, Q> &obj)
	{
		for (size_t i = 0; i < C; i++)
		{
			for (size_t j = 0; j < R; j++)
			{
				ar(obj[i][j]);
			}
		}
	}

	template <typename TArchive, length_t C, length_t R, typename T, qualifier Q>
	void Deserialize(TArchive &ar, mat<C, R, T, Q> &obj)
	{
		for (size_t i = 0; i < C; i++)
		{
			for (size_t j = 0; j < R; j++)
			{
				ar(obj[i][j]);

			}
		}
	}
}