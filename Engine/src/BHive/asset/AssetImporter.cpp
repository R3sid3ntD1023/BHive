#include "AssetImporter.h"
#include "FactoryRegistry.h"

namespace BHive
{
	void AssetImporter::CreateNew(const AssetType &type, const std::filesystem::path &path)
	{
		auto &registry = FactoryRegistry::Get();

		if (auto Factory = registry.Get(type))
		{
			if (Factory->CanCreateNew())
			{
				Factory->CreateNew(path);
				mOtherCreatedAssets = Factory->GetOtherCreatedAssets();
			}
		}
	}

	bool AssetImporter::Import(Ref<Asset> &asset, const FAssetMetaData &metadata)
	{
		if (auto Factory = FactoryRegistry::Get().Get(metadata.Type))
		{
			if (Factory->Import(asset, metadata.Path))
			{
				mOtherCreatedAssets = Factory->GetOtherCreatedAssets();

				return true;
			}

			return false;
		}

		LOG_ERROR("No valid Factory found for {}", metadata.Path.extension().string());
		LOG_ERROR("Create a Factory from asset/fentityies/Factory.h for the asset type"
				  "and then register the Factory with asset_registry");

		return false;
	}
}