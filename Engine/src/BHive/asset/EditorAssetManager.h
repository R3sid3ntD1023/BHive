#pragma once

#include "AssetManagerBase.h"
#include "AssetMetaData.h"
#include "Asset.h"
#include "AssetFactory.h"

namespace BHive
{
	using AssetRegistry = std::unordered_map<AssetHandle, FAssetMetaData>;

	class EditorAssetManager : public AssetManagerBase
	{
	public:
		EditorAssetManager(const std::filesystem::path &directory, const std::string &fileName);
		~EditorAssetManager();

		virtual Ref<Asset> GetAsset(AssetHandle handle) override;

		virtual bool IsAssetHandleValid(AssetHandle handle) const override;
		virtual bool IsAssetLoaded(AssetHandle handle) const override;
		virtual AssetType GetAssetType(AssetHandle handle) const override;

		void ImportAsset(const std::filesystem::path &path, const AssetType &type, const AssetHandle &handle);
		bool RemoveAsset(AssetHandle handle);
		bool RemoveAsset(const std::filesystem::path &path);
		bool RenameAsset(const std::filesystem::path &old_, const std::filesystem::path &new_);

		const FAssetMetaData &GetMetaData(AssetHandle handle) const;
		FAssetMetaData &GetMetaData(AssetHandle handle);
		const FAssetMetaData &GetMetaData(const std::filesystem::path &file) const;
		FAssetMetaData &GetMetaData(const std::filesystem::path &file);

		AssetHandle GetHandle(const std::filesystem::path &file) const;
		const std::filesystem::path &GetFilePath(AssetHandle handle) const;

		const AssetRegistry &GetAssetRegistry() const { return mAssetRegistry; }

		void Serialize() const;
		bool Deserialize();

	private:
		AssetRegistry mAssetRegistry;
		AssetMap mLoadedAssets;
		AssetMap mMemoryAssets;
		AssetMap mTempAssets;
		std::filesystem::path mDirectory;
		std::string mFileName;

		AssetFactory mAssetFactory;
	};
} // namespace BHive