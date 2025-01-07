#pragma once

#include "Asset.h"
#include "AssetMetaData.h"
#include "Factory.h"

namespace BHive
{
	class AssetImporter
	{
	public:

		void CreateNew(const AssetType& type, const std::filesystem::path& path);

		bool Import(Ref<Asset>& asset, const FAssetMetaData& metadata);

	};
}