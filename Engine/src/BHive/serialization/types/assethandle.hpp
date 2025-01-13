#pragma once

#include <cereal/cereal.hpp>
#include "asset/TAssetHandler.h"
#include "asset/AssetManager.h"

namespace BHive
{
	template <typename A, typename T>
	inline uint64_t CEREAL_SAVE_MINIMAL_FUNCTION_NAME(const A &ar, const TAssetHandle<T> &obj)
	{
		return obj ? obj.get()->GetHandle() : AssetHandle(0);
	}

	template <typename A, typename T>
	inline void CEREAL_LOAD_MINIMAL_FUNCTION_NAME(const A &ar, TAssetHandle<T> &obj, const uint64_t &value)
	{
		obj = AssetManager::GetAsset<T>(value);
	}
} // namespace BHive