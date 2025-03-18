#include "ResourceManager.h"
#include <core/serialization/Serialization.h>
#include <asset/AssetFactory.h>

namespace BHive
{
	ResourceManager::ResourceManager(const std::filesystem::path &directory, const std::string &filename)
		: mDirectory(directory),
		  mFileName(filename)
	{

		LoadRegistry();
	}

	bool ResourceManager::Import(const std::filesystem::path &path)
	{
		if (path.extension() == ".asset")
		{
			UUID id;

			auto &asset = mLoadedAssets[id];
			AssetFactory::Import(asset, path);

			FAssetMetaData metadata{};
			metadata.Type = asset->get_type();
			metadata.Path = std::filesystem::relative(path, mDirectory);
			metadata.Name = path.stem().string();

			mMetaData.emplace(id, metadata);
			SaveRegistry();

			return true;
		}

		return false;
	}

	Ref<Asset> ResourceManager::GetAsset(AssetHandle handle)
	{
		if (IsMemoryAsset(handle))
		{
			return mMemoryAssets.at(handle);
		}

		if (!IsAssetHandleValid(handle))
		{
			return nullptr;
		}

		if (IsAssetLoaded(handle))
		{
			return mLoadedAssets.at(handle);
		}

		Ref<Asset> &asset = mLoadedAssets[handle];

		auto &metadata = mMetaData.at(handle);
		AssetFactory::Import(asset, mDirectory / metadata.Path);

		return asset;
	}

	bool ResourceManager::IsAssetHandleValid(AssetHandle handle) const
	{
		return mMetaData.contains(handle);
	}

	bool ResourceManager::IsAssetLoaded(AssetHandle handle) const
	{
		return mLoadedAssets.contains(handle) && mLoadedAssets.at(handle);
	}

	AssetType ResourceManager::GetAssetType(AssetHandle handle) const
	{
		static FAssetMetaData null_data{};

		if (mMetaData.contains(handle))
			return mMetaData.at(handle).Type;

		return null_data.Type;
	}

	void ResourceManager::SaveRegistry()
	{
		if (!std::filesystem::exists(mDirectory))
		{
			std::filesystem::create_directory(mDirectory);
		}

		std::ofstream out(mDirectory / mFileName);
		cereal::JSONOutputArchive ar(out);
		ar(mMetaData);
	}

	void ResourceManager::LoadRegistry()
	{
		if (!std::filesystem::exists(mDirectory / mFileName))
		{
			return;
		}

		std::ifstream in(mDirectory / mFileName);
		cereal::JSONInputArchive ar(in);
		ar(mMetaData);
	}

	bool ResourceManager::IsMemoryAsset(AssetHandle handle) const
	{
		return mMemoryAssets.contains(handle);
	}
} // namespace BHive