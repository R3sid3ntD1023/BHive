#include "EditorAssetManager.h"
#include "asset/AssetImporter.h"
#include "FactoryRegistry.h"
#include "serialization/Serialization.h"
#include "threading/Threading.h"

namespace BHive
{
	AssetType GetAssetTypeFromExtenstion(const std::filesystem::path &extension)
	{
		return FactoryRegistry::Get().GetTypeFromExtension(extension.string().c_str());
	}

	EditorAssetManager::EditorAssetManager(const std::filesystem::path &path, bool save_registry)
		: mAssetRegistryPath(path), mSaveRegistry(save_registry)
	{
		if (std::filesystem::exists(path) && save_registry)
			DeserializeAssetRegistry();
	}

	EditorAssetManager::~EditorAssetManager()
	{
		LOG_TRACE("EditorAssetManager Destructor Called");
	}

	Ref<Asset> EditorAssetManager::GetAsset(AssetHandle handle)
	{
		Ref<Asset> asset;

		if (!IsAssetHandleValid(handle))
			return nullptr;

		if (mMemoryAssets.contains(handle))
		{
			asset = mMemoryAssets.at(handle);
		}
		else
		{
			if (IsAssetLoaded(handle))
			{
				asset = mLoadedAssets.at(handle);
			}
			else
			{
				AssetImporter importer;

				const FAssetMetaData &metadata = GetMetaData(handle);
				bool success = importer.Import(asset, metadata);
				if (!success)
				{
					LOG_ERROR("Failed to load asset");
					return asset;
				}

				asset->Handle = handle;
				mLoadedAssets[handle] = asset;
				LOG_TRACE("Imported asset {}", metadata.Path.string());
			}
		}

		return asset;
	}

	void EditorAssetManager::AddAsset(Ref<Asset> asset, AssetHandle handle, const std::string &name)
	{
		if (!mMemoryAssets.contains(handle))
		{
			mMemoryAssets.emplace(handle, asset);
			mAssetRegistry.emplace(handle, FAssetMetaData{.Type = asset->GetType(), .Path = name, .Name = name});
			asset->Handle = handle;
		}
	}

	bool EditorAssetManager::IsAssetHandleValid(AssetHandle handle) const
	{
		return (bool)handle && mAssetRegistry.contains(handle);
	}

	bool EditorAssetManager::IsAssetLoaded(AssetHandle handle) const
	{
		return mLoadedAssets.contains(handle);
	}

	AssetType EditorAssetManager::GetAssetType(AssetHandle handle) const
	{
		if (!IsAssetHandleValid(handle))
			return InvalidType;

		return mAssetRegistry.at(handle).Type;
	}

	AssetHandle EditorAssetManager::CreateNewAsset(const AssetType &type, const std::filesystem::path &path)
	{
		AssetImporter importer;
		importer.CreateNew(type, path);

		FAssetMetaData metadata;
		metadata.Path = path;
		metadata.Type = type;
		metadata.Name = path.stem().string();

		AssetHandle handle;
		mAssetRegistry[handle] = metadata;

		SerializeAssetRegistry();

		return handle;
	}

	AssetHandle EditorAssetManager::ImportAsset(const std::filesystem::path &path)
	{
		return ImportAsset(path, FactoryRegistry::Get().GetTypeFromExtension(path.extension().string()));
	}

	AssetHandle EditorAssetManager::ImportAsset(const std::filesystem::path &path, AssetType type)
	{
		if (auto handle = GetHandle(path))
		{
			return handle;
		}

		FAssetMetaData metadata;
		metadata.Path = path;
		metadata.Type = type;
		metadata.Name = path.stem().string();
		if (metadata.Type == InvalidType)
		{
			LOG_ERROR("UnSupported Asset Type");
			return 0;
		}

		AssetHandle handle;
		mAssetRegistry[handle] = metadata;
		SerializeAssetRegistry();

		return handle;
	}

	bool EditorAssetManager::RemoveAsset(AssetHandle handle)
	{
		if (mAssetRegistry.contains(handle))
		{
			mAssetRegistry.erase(handle);
			SerializeAssetRegistry();
			return true;
		}

		if (mLoadedAssets.contains(handle))
		{
			mLoadedAssets.erase(handle);
			return true;
		}

		return false;
	}

	bool EditorAssetManager::RemoveAsset(const std::filesystem::path &path)
	{
		auto handle = GetHandle(path);
		if (!handle)
			return false;

		return RemoveAsset(handle);
	}

	bool EditorAssetManager::RenameAsset(const std::filesystem::path &old_, const std::filesystem::path &new_)
	{
		auto &metadata = GetMetaData(old_);
		if (!metadata)
			return false;

		metadata.Path = new_;
		metadata.Name = new_.filename().string();

		SerializeAssetRegistry();

		return true;
	}

	const FAssetMetaData &EditorAssetManager::GetMetaData(AssetHandle handle) const
	{
		static FAssetMetaData sNullMetaData;
		if (mAssetRegistry.contains(handle))
		{
			return mAssetRegistry.at(handle);
		}

		return sNullMetaData;
	}

	FAssetMetaData &EditorAssetManager::GetMetaData(AssetHandle handle)
	{
		static FAssetMetaData sNullMetaData;
		if (mAssetRegistry.contains(handle))
		{
			return mAssetRegistry.at(handle);
		}

		return sNullMetaData;
	}

	const FAssetMetaData &EditorAssetManager::GetMetaData(const std::filesystem::path &file) const
	{
		static FAssetMetaData sNullMetaData;
		auto it = std::find_if(mAssetRegistry.begin(), mAssetRegistry.end(), [file](const auto &pair)
							   { return pair.second.Path == file; });

		if (it != mAssetRegistry.end())
			return (*it).second;

		return sNullMetaData;
	}

	FAssetMetaData &EditorAssetManager::GetMetaData(const std::filesystem::path &file)
	{
		static FAssetMetaData sNullMetaData;
		auto it = std::find_if(mAssetRegistry.begin(), mAssetRegistry.end(), [file](const auto &pair)
							   { return pair.second.Path == file; });

		if (it != mAssetRegistry.end())
			return (*it).second;

		return sNullMetaData;
	}

	AssetHandle EditorAssetManager::GetHandle(const std::filesystem::path &file) const
	{
		auto it = std::find_if(mAssetRegistry.begin(), mAssetRegistry.end(), [file](const auto &pair)
							   { return pair.second.Path == file; });

		if (it != mAssetRegistry.end())
			return (*it).first;

		return 0;
	}

	const std::filesystem::path &EditorAssetManager::GetFilePath(AssetHandle handle) const
	{
		return GetMetaData(handle).Path;
	}

	void EditorAssetManager::SerializeAssetRegistry() const
	{
		if (!mSaveRegistry)
			return;

		FileStreamWriter ar(mAssetRegistryPath);
		ar(mAssetRegistry);
	}

	bool EditorAssetManager::DeserializeAssetRegistry()
	{
		if (!std::filesystem::exists(mAssetRegistryPath))
			return false;

		FileStreamReader ar(mAssetRegistryPath);
		ar(mAssetRegistry);

		return true;
	}
}