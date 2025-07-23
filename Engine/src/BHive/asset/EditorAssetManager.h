#pragma once

#include "AssetManagerBase.h"
#include "AssetMetaData.h"
#include "Asset.h"
#include "AssetFactory.h"

namespace BHive
{
	using AssetRegistry = std::unordered_map<UUID, FAssetMetaData>;

	class BHIVE_API EditorAssetManager : public AssetManagerBase
	{
	public:
		EditorAssetManager(const std::filesystem::path &directory, const std::string &fileName);
		~EditorAssetManager();

		virtual Ref<Asset> GetAsset(UUID handle) override;

		virtual bool IsAssetHandleValid(UUID handle) const override;
		virtual bool IsAssetLoaded(UUID handle) const override;
		virtual rttr::type GetAssetType(UUID handle) const override;

		void ImportAsset(const std::filesystem::path &path, const rttr::type &type, const UUID &handle);
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
		AssetRegistry mAssetRegistry;
		AssetMap mLoadedAssets;
		AssetMap mMemoryAssets;
		AssetMap mTempAssets;
		std::filesystem::path mDirectory;
		std::string mFileName;

		AssetFactory mAssetFactory;
	};
} // namespace BHive