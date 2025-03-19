#pragma once

#include "AssetManagerBase.h"

namespace BHive
{
	class Asset;

	class AssetManager
	{
	public:
		static BHIVE Ref<Asset> GetAsset(AssetHandle handle);

		template <typename T>
		static Ref<T> GetAsset(AssetHandle handle)
		{
			auto asset = sCurrentAssetManager->GetAsset(handle);
			return std::dynamic_pointer_cast<T>(asset);
		}

		static BHIVE bool IsAssetHandleValid(AssetHandle handle);

		static BHIVE bool IsAssetLoaded(AssetHandle handle);

		static BHIVE AssetType GetAssetType(AssetHandle handle);

		static void SetAssetManager(AssetManagerBase *manager);

		static AssetManagerBase *GetAssetManager() { return sCurrentAssetManager; };

		template <typename T>
		static T *GetAssetManager()
		{
			return Cast<T>(sCurrentAssetManager);
		}

	private:
		static AssetManagerBase *sCurrentAssetManager;
	};
} // namespace BHive