#pragma once

#include "math/Math.h"
#include <cereal/archives/binary.hpp>
#include <cereal/cereal.hpp>
#include <string>

template <typename A>
constexpr bool is_json_archive_v = std::is_same_v<A, cereal::JSONOutputArchive> || std::is_same_v<A, cereal::JSONInputArchive>;

template <typename A>
constexpr bool is_binary_archive_v = std::is_same_v<A, cereal::BinaryOutputArchive> || std::is_same_v<A, cereal::BinaryInputArchive>;

namespace glm
{

	template <typename A, length_t C, length_t R, typename T, qualifier Q, std::enable_if_t<is_json_archive_v<A>, bool> = true>
	inline void CEREAL_SERIALIZE_FUNCTION_NAME(A &ar, mat<C, R, T, Q> &obj)
	{
		size_t size = C * R;
		ar(cereal::make_size_tag(size));
		for (int i = 0; i < C; i++)
		{
			for (int j = 0; j < R; j++)
			{
				ar(obj[i][j]);
			}
		}
	}

	template <typename A, typename T, qualifier Q, std::enable_if_t<is_json_archive_v<A>, bool> = true>
	inline void CEREAL_SERIALIZE_FUNCTION_NAME(A &ar, qua<T, Q> &obj)
	{
		size_t size = 4;
		ar(cereal::make_size_tag(size));
		ar(obj.x, obj.y, obj.z, obj.w);
	}

	template <typename A, length_t L, typename T, qualifier Q, std::enable_if_t<is_json_archive_v<A>, bool> = true>
	inline void CEREAL_SERIALIZE_FUNCTION_NAME(A &ar, vec<L, T, Q> &obj)
	{
		size_t size = L;
		ar(cereal::make_size_tag(size));
		for (int i = 0; i < L; i++)
		{
			ar(obj[i]);
		}
	}

	template <typename A, length_t C, length_t R, typename T, qualifier Q, std::enable_if_t<is_binary_archive_v<A>, bool> = true>
	inline void CEREAL_SERIALIZE_FUNCTION_NAME(A &ar, mat<C, R, T, Q> &obj)
	{
		ar(cereal::binary_data(&obj, sizeof(obj)));
	}

	template <typename A, typename T, qualifier Q, std::enable_if_t<is_binary_archive_v<A>, bool> = true>
	inline void CEREAL_SERIALIZE_FUNCTION_NAME(A &ar, qua<T, Q> &obj)
	{
		ar(cereal::binary_data(&obj, sizeof(obj)));
	}

	template <typename A, length_t L, typename T, qualifier Q, std::enable_if_t<is_binary_archive_v<A>, bool> = true>
	inline void CEREAL_SERIALIZE_FUNCTION_NAME(A &ar, vec<L, T, Q> &obj)
	{
		ar(cereal::binary_data(&obj, sizeof(obj)));
	}

} // namespace glm