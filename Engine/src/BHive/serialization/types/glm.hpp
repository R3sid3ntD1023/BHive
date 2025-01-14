#pragma once

#include <glm/glm.hpp>
#include <string>

namespace glm
{

	template <typename TArchive, length_t L, typename T, qualifier Q>
	void Serialize(TArchive &ar, const vec<L, T, Q> &obj)
	{
<<<<<<< HEAD
		std::stringstream ss;
		ss << '[';
=======
>>>>>>> parent of 1c2673b (removed custom serializer and added cereal)
		for (size_t i = 0; i < L; i++)
		{
			ar(obj[i]);
		}
<<<<<<< HEAD

		ss << ']';

		return ss.str();
=======
>>>>>>> parent of 1c2673b (removed custom serializer and added cereal)
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
<<<<<<< HEAD
		std::stringstream ss;
		ss << '[';
=======
>>>>>>> parent of 1c2673b (removed custom serializer and added cereal)
		for (size_t i = 0; i < C; i++)
		{
			for (size_t j = 0; j < R; j++)
			{
				ar(obj[i][j]);
			}
		}
<<<<<<< HEAD

		ss << ']';

		return ss.str();
=======
>>>>>>> parent of 1c2673b (removed custom serializer and added cereal)
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
<<<<<<< HEAD

	template <typename A, typename T, qualifier Q>
	inline std::string CEREAL_SAVE_MINIMAL_FUNCTION_NAME(A &ar, const qua<T, Q> &q)
	{
		return std::format("[{}, {}, {}, {}]", q.x, q.y, q.z, q.w);
	}

	template <typename A, typename T, qualifier Q>
	inline void CEREAL_LOAD_MINIMAL_FUNCTION_NAME(A &ar, qua<T, Q> &q, const std::string &value)
	{
		char token;
		std::stringstream ss(value);
		ss >> token >> q.x >> token >> q.y >> token >> q.z >> token >> q.w >> token;

	} // namespace glm

} // namespace glm
=======
}
>>>>>>> parent of 1c2673b (removed custom serializer and added cereal)
