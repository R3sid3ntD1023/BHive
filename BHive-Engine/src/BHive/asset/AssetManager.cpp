#include "AssetManager.h"
#include "Asset.h"

namespace BHive
{
	Ref<Asset> AssetManager::GetAsset(UUID handle)
	{
		if (!sCurrentAssetManager)
			return nullptr;
		auto asset = sCurrentAssetManager->GetAsset(handle);
		return asset;
	}

	bool AssetManager::IsAssetHandleValid(UUID handle)
	{
		if (!sCurrentAssetManager)
			return false;

		return sCurrentAssetManager->IsAssetHandleValid(handle);
	}

	bool AssetManager::IsAssetLoaded(UUID handle)
	{
		if (!sCurrentAssetManager)
			return false;

		return sCurrentAssetManager->IsAssetLoaded(handle);
	}

	rttr::type AssetManager::GetAssetType(UUID handle)
	{
		if (!sCurrentAssetManager)
			return rttr::type::get<void>();
		return sCurrentAssetManager->GetAssetType(handle);
	}

	void AssetManager::SetAssetManager(AssetManagerBase *manager)
	{
		sCurrentAssetManager = manager;
	}

} // namespace BHive
