#pragma once

#include "Asset.h"
#include "AssetFactory.h"
#include "AssetManagerBase.h"
#include "AssetMetaData.h"

namespace BHive
{
	using AssetRegistry = std::unordered_map<UUID, FAssetMetaData>;

	class BHIVE_API EditorAssetManager : public AssetManagerBase
	{
	public:
		EditorAssetManager(const std::filesystem::path &directory, const std::string &fileName);
		~EditorAssetManager();

		Ref<Asset> GetAsset(UUID handle) override;

		bool IsAssetHandleValid(UUID handle) const override;
		bool IsAssetLoaded(UUID handle) const override;
		rttr::type GetAssetType(UUID handle) const override;

		UUID ImportAsset(const std::filesystem::path &path, const rttr::type &type);
		bool RemoveAsset(UUID handle);
		bool RemoveAsset(const std::filesystem::path &relative_path);
		bool RenameAsset(const std::filesystem::path &old_, const std::filesystem::path &new_);

		const FAssetMetaData &GetMetaData(UUID handle) const;
		FAssetMetaData &GetMetaData(UUID handle);
		const FAssetMetaData &GetMetaData(const std::filesystem::path &file) const;
		FAssetMetaData &GetMetaData(const std::filesystem::path &file);

		UUID GetHandle(const std::filesystem::path &file) const;
		const std::filesystem::path &GetFilePath(UUID handle) const;

		const AssetRegistry &GetAssetRegistry() const { return mAssetRegistry; }

		void Serialize() const;
		bool Deserialize();

	private:
		std::filesystem::path GetDirectory() const;

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