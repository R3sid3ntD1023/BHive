#pragma once

#include <asset/AssetManagerBase.h>
#include <asset/AssetMetaData.h>

namespace BHive
{
	class ResourceManager : public AssetManagerBase
	{
	public:
		ResourceManager(const std::filesystem::path &directory);

		bool Import(const std::filesystem::path &path);

		virtual Ref<Asset> GetAsset(AssetHandle handle) override;

		virtual bool IsAssetHandleValid(AssetHandle handle) const override;
		virtual bool IsAssetLoaded(AssetHandle handle) const override;
		virtual AssetType GetAssetType(AssetHandle handle) const override;

	private:
		void SaveRegistry();
		void LoadRegistry();
		void RegisterImporters();
		void RegisterDefaultAssets();
		bool IsMemoryAsset(AssetHandle handle) const;

	private:
		std::filesystem::path mDirectory;
		std::unordered_map<AssetHandle, FAssetMetaData> mMetaData;
		std::unordered_map<rttr::type, std::function<Ref<Asset>(const std::filesystem::path &)>> mLoadFunctions;
		std::unordered_map<AssetHandle, Ref<Asset>> mLoadedAssets;
		std::unordered_map<AssetHandle, Ref<Asset>> mMemoryAssets;
	};
} // namespace BHive