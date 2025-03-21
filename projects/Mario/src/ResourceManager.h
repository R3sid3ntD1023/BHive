#pragma once

#include <asset/AssetManagerBase.h>
#include <asset/AssetMetaData.h>

namespace BHive
{
	class ResourceManager : public AssetManagerBase
	{
	public:
		ResourceManager(const std::filesystem::path &directory, const std::string &filename);

		bool Import(const std::filesystem::path &path);

		virtual Ref<Asset> GetAsset(UUID handle) override;

		virtual bool IsAssetHandleValid(UUID handle) const override;
		virtual bool IsAssetLoaded(UUID handle) const override;
		virtual AssetType GetAssetType(UUID handle) const override;

	private:
		void SaveRegistry();
		void LoadRegistry();
		bool IsMemoryAsset(UUID handle) const;

	private:
		std::filesystem::path mDirectory;
		std::string mFileName;
		std::unordered_map<UUID, FAssetMetaData> mMetaData;
		std::unordered_map<rttr::type, std::function<Ref<Asset>(const std::filesystem::path &)>> mLoadFunctions;
		std::unordered_map<UUID, Ref<Asset>> mLoadedAssets;
		std::unordered_map<UUID, Ref<Asset>> mMemoryAssets;
	};
} // namespace BHive