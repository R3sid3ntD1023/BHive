#pragma once

#include "AssetManagerBase.h"

namespace BHive
{
	class Asset;

	class AssetManager
	{
	public:
		static BHIVE Ref<Asset> GetAsset(UUID handle);

		template <typename T>
		static Ref<T> GetAsset(UUID handle)
		{
			auto asset = sCurrentAssetManager->GetAsset(handle);
			return std::dynamic_pointer_cast<T>(asset);
		}

		static BHIVE bool IsAssetHandleValid(UUID handle);

		static BHIVE bool IsAssetLoaded(UUID handle);

		static BHIVE AssetType GetAssetType(UUID handle);

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