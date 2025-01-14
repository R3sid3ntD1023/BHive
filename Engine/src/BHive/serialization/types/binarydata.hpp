#pragma once

#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>

namespace cereal
{
<<<<<<< HEAD

	template <typename T>
	inline void
	CEREAL_SAVE_FUNCTION_NAME(cereal::JSONOutputArchive &ar, const cereal::BinaryData<T> &obj)
	{
		ar.saveBinaryValue(obj.data, obj.size, "Data");
=======
	template<typename T>
	inline void CEREAL_SAVE_FUNCTION_NAME(cereal::JSONOutputArchive& ar, const cereal::BinaryData<T>& obj)
	{
		ar.saveBinaryValue(obj.data, obj.size);
>>>>>>> parent of 7ce9339 (reverted commit)
	}

	template <typename T>
	inline void CEREAL_LOAD_FUNCTION_NAME(cereal::JSONInputArchive &ar, cereal::BinaryData<T> &obj)
	{
<<<<<<< HEAD
		ar.loadBinaryValue(obj.data, obj.size, "Data");
	}
} // namespace cereal
=======
		ar.loadBinaryValue(obj.data, obj.size);
	}
}
>>>>>>> parent of 7ce9339 (reverted commit)
