#pragma once

#include "core/Core.h"
#include "core/UUID.h"

namespace BHive
{
	class Asset;

	using AssetMap = std::unordered_map<UUID, Ref<Asset>>;

	class BHIVE_API AssetManagerBase
	{
	public:
		virtual Ref<Asset> GetAsset(UUID handle) = 0;

		virtual bool IsAssetHandleValid(UUID handle) const = 0;
		virtual bool IsAssetLoaded(UUID handle) const = 0;
		virtual rttr::type GetAssetType(UUID handle) const = 0;
	};
} // namespace BHive