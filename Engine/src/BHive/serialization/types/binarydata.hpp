#pragma once

#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>

namespace cereal
{

	template <typename T>
	inline void
	CEREAL_SAVE_FUNCTION_NAME(cereal::JSONOutputArchive &ar, const cereal::BinaryData<T> &obj)
	{
		ar.saveBinaryValue(obj.data, obj.size, "Data");
	}

	template <typename T>
	inline void CEREAL_LOAD_FUNCTION_NAME(cereal::JSONInputArchive &ar, cereal::BinaryData<T> &obj)
	{
		ar.loadBinaryValue(obj.data, obj.size, "Data");
	}
} // namespace cereal