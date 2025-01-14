#pragma once

#include <cereal/cereal.hpp>
#include "asset/AssetMetaData.h"

namespace BHive
{
	template <typename A>
	inline void CEREAL_SERIALIZE_FUNCTION_NAME(A &ar, FAssetMetaData &obj)
	{
		ar(cereal::make_nvp("Type", obj.Type), cereal::make_nvp("Path", obj.Path), cereal::make_nvp("Name", obj.Name));
	}
} // namespace BHive
