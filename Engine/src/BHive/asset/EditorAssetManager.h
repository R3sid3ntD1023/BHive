#pragma once

#include "asset/AssetManagerBase.h"
#include "asset/AssetMetaData.h"
#include "asset/Asset.h"
#include "asset/Factory.h"

namespace BHive
{
	using AssetRegistry = std::map<AssetHandle, FAssetMetaData>;

	class EditorAssetManager : public AssetManagerBase
	{
	public:
		EditorAssetManager(const std::filesystem::path &path, bool save_registry = true);
		~EditorAssetManager();

		virtual Ref<Asset> GetAsset(AssetHandle handle) override;
		virtual void AddAsset(Ref<Asset> asset, AssetHandle handle, const std::string &name) override;

		virtual bool IsAssetHandleValid(AssetHandle handle) const override;
		virtual bool IsAssetLoaded(AssetHandle handle) const override;
		virtual AssetType GetAssetType(AssetHandle handle) const override;

		AssetHandle CreateNewAsset(const AssetType &type, const std::filesystem::path &path);

		AssetHandle ImportAsset(const std::filesystem::path &path);

		template <typename T>
		AssetHandle ImportAsset(const std::filesystem::path &path)
		{
			return ImportAsset(path, T::GetStaticType());
		}

		AssetHandle ImportAsset(const std::filesystem::path &path, AssetType type);
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

		void SerializeAssetRegistry() const;
		bool DeserializeAssetRegistry();

	private:
		AssetRegistry mAssetRegistry;
		AssetMap mLoadedAssets;
		AssetMap mMemoryAssets;
		AssetMap mTempAssets;
		std::filesystem::path mAssetRegistryPath;
		bool mSaveRegistry;
	};
}