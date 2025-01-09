#include "AssetManager.h"
#include "Asset.h"

namespace BHive
{
    Ref<Asset> AssetManager::GetAsset(AssetHandle handle)
    {
        auto asset = sCurrentAssetManager->GetAsset(handle);
        return asset;
    }

    bool AssetManager::IsAssetHandleValid(AssetHandle handle)
    {
        return sCurrentAssetManager->IsAssetHandleValid(handle);
    }

    bool AssetManager::IsAssetLoaded(AssetHandle handle)
    {
        return sCurrentAssetManager->IsAssetLoaded(handle);
    }

    AssetType AssetManager::GetAssetType(AssetHandle handle)
    {
        return sCurrentAssetManager->GetAssetType(handle);
    }

    void AssetManager::SetAssetManager(AssetManagerBase *manager)
    {
        sCurrentAssetManager = manager;
    }

    AssetManagerBase *AssetManager::sCurrentAssetManager = nullptr;
} // namespace BHive
