#include "AssetImporter.h"
#include "FactoryRegistry.h"

namespace BHive
{
	void AssetImporter::CreateNew(const AssetType &type, const std::filesystem::path &path)
	{
		auto &registry = FactoryRegistry::Get();

		if (auto factory = registry.Get(type))
		{
			if (factory->CanCreateNew())
			{
				factory->CreateNew(path);
				mOtherCreatedAssets = factory->GetOtherCreatedAssets();
			}
		}
	}

	bool AssetImporter::Import(Ref<Asset> &asset, const FAssetMetaData &metadata)
	{
		if (auto factory = FactoryRegistry::Get().Get(metadata.Type))
		{
			if (factory->Import(asset, metadata.Path))
			{
				mOtherCreatedAssets = factory->GetOtherCreatedAssets();

				return true;
			}

			return false;
		}

		LOG_ERROR("No valid factory found for {}", metadata.Path.extension().string());
		LOG_ERROR("Create a factory from asset/factories/Factory.h for the asset type"
				  "and then register the factory with asset_registry");

		return false;
	}
}