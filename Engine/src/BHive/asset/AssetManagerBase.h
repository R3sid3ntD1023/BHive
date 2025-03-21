#pragma once

#include "core/UUID.h"
#include "asset/AssetType.h"

namespace BHive
{
	class Asset;

	using AssetMap = std::unordered_map<UUID, Ref<Asset>>;

	class AssetManagerBase
	{
	public:
		virtual Ref<Asset> GetAsset(UUID handle) = 0;

		virtual bool IsAssetHandleValid(UUID handle) const = 0;
		virtual bool IsAssetLoaded(UUID handle) const = 0;
		virtual AssetType GetAssetType(UUID handle) const = 0;
	};
}