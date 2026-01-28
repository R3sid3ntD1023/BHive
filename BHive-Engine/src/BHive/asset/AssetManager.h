#pragma once

#include "AssetManagerBase.h"

namespace BHive
{
	class Asset;

	class BHIVE_API AssetManager
	{
	public:
		static Ref<Asset> GetAsset(UUID handle);

		template <typename T>
		static Ref<T> GetAsset(UUID handle)
		{
			auto asset = sCurrentAssetManager->GetAsset(handle);
			return std::dynamic_pointer_cast<T>(asset);
		}

		static bool IsAssetHandleValid(UUID handle);

		static bool IsAssetLoaded(UUID handle);

		static rttr::type GetAssetType(UUID handle);

		static void SetAssetManager(AssetManagerBase *manager);

		static AssetManagerBase *GetAssetManager() { return sCurrentAssetManager; };

		template <typename T>
		static T *GetAssetManager()
		{
			return Cast<T>(sCurrentAssetManager);
		}

	private:
		static inline AssetManagerBase *sCurrentAssetManager = nullptr;
	};
} // namespace BHive