#pragma once

#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>

namespace cereal
{
	template <typename T>
	void CEREAL_SAVE_FUNCTION_NAME(cereal::JSONOutputArchive &ar, const BinaryData<T> &d)
	{
		ar.saveBinaryValue(d.data, d.size);
	}

	template <typename T>
	void CEREAL_LOAD_FUNCTION_NAME(cereal::JSONInputArchive &ar, BinaryData<T> &d)
	{
		ar.loadBinaryValue(d.data, d.size);
	}
} // namespace cereal