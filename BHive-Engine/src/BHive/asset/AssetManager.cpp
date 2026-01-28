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

	rttr::type AssetManager::GetAssetType(UUID handle)
	{
		return sCurrentAssetManager->GetAssetType(handle);
	}

	void AssetManager::SetAssetManager(AssetManagerBase *manager)
	{
		sCurrentAssetManager = manager;
	}

} // namespace BHive
