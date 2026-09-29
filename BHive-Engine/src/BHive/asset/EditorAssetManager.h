#pragma once

#include "Asset.h"
#include "AssetManagerBase.h"
#include "AssetMetaData.h"
#include "AssetSerializer.h"
#include "FactoryRegistry.h"

namespace BHive
{
	using AssetRegistry = std::unordered_map<UUID, FAssetMetaData>;
	using PathUUIDMap = std::unordered_map<std::filesystem::path, UUID>;

	class BHIVE_API EditorAssetManager : public AssetManagerBase
	{
	public:
		EditorAssetManager(const std::filesystem::path &directory);

		~EditorAssetManager();

		Ref<Asset> GetAsset(UUID handle) override;

		bool IsAssetHandleValid(UUID handle) const override;

		bool IsAssetLoaded(UUID handle) const override;

		rttr::type GetAssetType(UUID handle) const override;

		UUID ImportAsset(const std::filesystem::path &sourcePath);

		void ReimportAsset(UUID handle);

		bool RemoveAsset(UUID handle);

		bool RemoveAsset(const std::filesystem::path &file);

		bool RenameAsset(const std::filesystem::path &old_, const std::filesystem::path &new_);

		FAssetMetaData &GetMetaData(UUID handle);

		const FAssetMetaData &GetMetaData(UUID handle) const;

		FAssetMetaData &GetMetaData(const std::filesystem::path &file);

		const FAssetMetaData &GetMetaData(const std::filesystem::path &file) const;

		UUID GetHandle(const std::filesystem::path &file) const;

		const std::filesystem::path &GetFilePath(UUID handle) const;

		const AssetRegistry &GetAssetRegistry() const { return mAssetRegistry; }

		bool IsAssetOutofDate(const FAssetMetaData &metaData) const;

	private:
		void Serialize(const FAssetMetaData &metaData) const;

		void Deserialize();

	private:
		std::filesystem::path GetDirectory() const;

		std::filesystem::path GetAssetPath(const std::string &name) const;

		std::filesystem::path GetMetaDataPath(const FAssetMetaData &metaData) const;

	private:
		AssetRegistry mAssetRegistry;

		PathUUIDMap mPathToHandle;

		AssetMap mLoadedAssets;

		AssetMap mMemoryAssets;

		AssetMap mTempAssets;

		std::filesystem::path mDirectory;

		AssetSerializer mAssetSerializer;

		FactoryRegistry mFactoryRegistry;
	};
} // namespace BHive