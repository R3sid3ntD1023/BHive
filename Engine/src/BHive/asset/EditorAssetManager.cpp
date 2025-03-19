#include "EditorAssetManager.h"
#include "core/serialization/Serialization.h"

namespace BHive
{

	EditorAssetManager::EditorAssetManager(const std::filesystem::path &directory, const std::string &filename)
		: mDirectory(directory),
		  mFileName(filename)
	{

		Deserialize();
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
				const FAssetMetaData &metadata = GetMetaData(handle);

				if (!mAssetFactory.Import(asset, mDirectory / metadata.Path))
				{
					LOG_ERROR("Failed to load asset");
					return asset;
				}

				mLoadedAssets[handle] = asset;
				LOG_TRACE("Imported asset {}", metadata.Path.string());
			}
		}

		return asset;
	}

	bool EditorAssetManager::IsAssetHandleValid(AssetHandle handle) const
	{
		return (bool)handle && mAssetRegistry.contains(handle);
	}

	bool EditorAssetManager::IsAssetLoaded(AssetHandle handle) const
	{
		bool loaded = mLoadedAssets.contains(handle);
		return loaded;
	}

	AssetType EditorAssetManager::GetAssetType(AssetHandle handle) const
	{
		if (!IsAssetHandleValid(handle))
			return InvalidType;

		return mAssetRegistry.at(handle).Type;
	}

	void EditorAssetManager::ImportAsset(const std::filesystem::path &path, const AssetType &type, const AssetHandle &handle)
	{
		if (GetHandle(path))
		{
			return;
		}

		if (type == InvalidType)
		{
			LOG_ERROR("UnSupported Asset Type");
			return;
		}

		FAssetMetaData metadata;
		metadata.Path = std::filesystem::relative(path, mDirectory);
		metadata.Type = type;
		metadata.Name = path.stem().string();

		mAssetRegistry[handle] = metadata;
		Serialize();
	}

	bool EditorAssetManager::RemoveAsset(AssetHandle handle)
	{
		if (mAssetRegistry.contains(handle))
		{
			mAssetRegistry.erase(handle);
			Serialize();
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
		metadata.Name = new_.stem().string();

		if (auto handle = GetHandle(old_))
		{
			if (IsAssetLoaded(handle))
			{
				mLoadedAssets[handle]->SetName(metadata.Name);
			}
		}

		Serialize();

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
		auto it = std::find_if(
			mAssetRegistry.begin(), mAssetRegistry.end(), [file](const auto &pair) { return pair.second.Path == file; });

		if (it != mAssetRegistry.end())
			return (*it).second;

		return sNullMetaData;
	}

	FAssetMetaData &EditorAssetManager::GetMetaData(const std::filesystem::path &file)
	{
		static FAssetMetaData sNullMetaData;
		auto it = std::find_if(
			mAssetRegistry.begin(), mAssetRegistry.end(), [file](const auto &pair) { return pair.second.Path == file; });

		if (it != mAssetRegistry.end())
			return (*it).second;

		return sNullMetaData;
	}

	AssetHandle EditorAssetManager::GetHandle(const std::filesystem::path &file) const
	{
		auto it = std::find_if(
			mAssetRegistry.begin(), mAssetRegistry.end(), [file](const auto &pair) { return pair.second.Path == file; });

		if (it != mAssetRegistry.end())
			return (*it).first;

		return UUID::Null;
	}

	const std::filesystem::path &EditorAssetManager::GetFilePath(AssetHandle handle) const
	{
		return GetMetaData(handle).Path;
	}

	void EditorAssetManager::Serialize() const
	{
		try
		{
			if (!std::filesystem::exists(mDirectory))
			{
				std::filesystem::create_directory(mDirectory);
			}

			std::ofstream out(mDirectory / mFileName, std::ios::out);
			if (!out)
				return;

			cereal::JSONOutputArchive ar(out);
			ar(MAKE_NVP("Assets", mAssetRegistry));
		}
		catch (std::exception &e)
		{
			LOG_ERROR("EditorAssetManager::Serialize() ERROR: {}", e.what());
		}
	}

	bool EditorAssetManager::Deserialize()
	{
		try
		{
			if (!std::filesystem::exists(mDirectory))
				return false;

			std::ifstream in(mDirectory / mFileName, std::ios::in);
			if (!in)
				return false;

			cereal::JSONInputArchive ar(in);
			ar(MAKE_NVP("Assets", mAssetRegistry));

			return true;
		}
		catch (std::exception &e)
		{
			LOG_ERROR("EditorAssetManager::Deserialize() ERROR: {}", e.what());
		}

		return false;
	}
} // namespace BHive