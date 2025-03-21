#include "AssetManager.h"
#include "Asset.h"

namespace BHive
{
	Ref<Asset> AssetManager::GetAsset(UUID handle)
	{
		auto asset = sCurrentAssetManager->GetAsset(handle);
		return asset;
	}

	bool AssetManager::IsAssetHandleValid(UUID handle)
	{
		return sCurrentAssetManager->IsAssetHandleValid(handle);
	}

	bool AssetManager::IsAssetLoaded(UUID handle)
	{
		return sCurrentAssetManager->IsAssetLoaded(handle);
	}

	AssetType AssetManager::GetAssetType(UUID handle)
	{
		return sCurrentAssetManager->GetAssetType(handle);
	}

	void AssetManager::SetAssetManager(AssetManagerBase *manager)
	{
		sCurrentAssetManager = manager;
	}

	AssetManagerBase *AssetManager::sCurrentAssetManager = nullptr;
} // namespace BHive
