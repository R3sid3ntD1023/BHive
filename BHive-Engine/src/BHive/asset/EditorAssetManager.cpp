#include "EditorAssetManager.h"

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

	Ref<Asset> EditorAssetManager::GetAsset(UUID handle)
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

				if (!mAssetFactory.Import(asset, metadata.Path))
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

	bool EditorAssetManager::IsAssetHandleValid(UUID handle) const
	{
		return (bool)handle && mAssetRegistry.contains(handle);
	}

	bool EditorAssetManager::IsAssetLoaded(UUID handle) const
	{
		bool loaded = mLoadedAssets.contains(handle);
		return loaded;
	}

	rttr::type EditorAssetManager::GetAssetType(UUID handle) const
	{
		if (!IsAssetHandleValid(handle))
			return InvalidType;

		return mAssetRegistry.at(handle).Type;
	}

	UUID EditorAssetManager::ImportAsset(const std::filesystem::path &path, const rttr::type &type)
	{
		if (auto handle = GetHandle(path))
		{
			return handle;
		}

		if (type == InvalidType)
		{
			LOG_ERROR("UnSupported Asset Type");
			return {NullID};
		}

		auto importPath = path.is_absolute() ? path : std::filesystem::relative(path, GetDirectory());
		FAssetMetaData metadata{};
		metadata.Path = GetDirectory() / path.filename();
		metadata.Type = type;
		metadata.Name = importPath.stem().string();

		UUID handle{};
		mAssetRegistry[handle] = metadata;
		Serialize();

		return handle;
	}

	bool EditorAssetManager::RemoveAsset(UUID handle)
	{
		bool removed = false;

		if (mAssetRegistry.contains(handle))
		{
			mAssetRegistry.erase(handle);
			Serialize();
			removed |= true;
		}

		if (mLoadedAssets.contains(handle))
		{
			mLoadedAssets.erase(handle);
			removed |= true;
		}

		return removed;
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

	const FAssetMetaData &EditorAssetManager::GetMetaData(UUID handle) const
	{
		static FAssetMetaData sNullMetaData;
		if (mAssetRegistry.contains(handle))
		{
			return mAssetRegistry.at(handle);
		}

		return sNullMetaData;
	}

	FAssetMetaData &EditorAssetManager::GetMetaData(UUID handle)
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
		auto it = std::find_if(mAssetRegistry.begin(), mAssetRegistry.end(), [file](const auto &pair) { return pair.second.Path == file; });

		if (it != mAssetRegistry.end())
			return (*it).second;

		return sNullMetaData;
	}

	FAssetMetaData &EditorAssetManager::GetMetaData(const std::filesystem::path &file)
	{
		static FAssetMetaData sNullMetaData;
		auto it = std::find_if(mAssetRegistry.begin(), mAssetRegistry.end(), [file](const auto &pair) { return pair.second.Path == file; });

		if (it != mAssetRegistry.end())
			return (*it).second;

		return sNullMetaData;
	}

	UUID EditorAssetManager::GetHandle(const std::filesystem::path &relative_path) const
	{
		auto it = std::find_if(mAssetRegistry.begin(), mAssetRegistry.end(), [=](const auto &pair) { return pair.second.Path == relative_path; });

		if (it != mAssetRegistry.end())
			return it->first;

		return NullID;
	}

	const std::filesystem::path &EditorAssetManager::GetFilePath(UUID handle) const
	{
		return GetMetaData(handle).Path;
	}

	void EditorAssetManager::Serialize() const
	{
		try
		{
			auto directory = GetDirectory();
			if (!std::filesystem::exists(directory))
			{
				std::filesystem::create_directory(directory);
			}

			std::ofstream out(directory / mFileName, std::ios::out);
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
			auto directory = GetDirectory();
			if (!std::filesystem::exists(directory))
				return false;

			std::ifstream in(directory / mFileName, std::ios::in);
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

	std::filesystem::path EditorAssetManager::GetDirectory() const
	{
		return !mDirectory.empty() ? mDirectory : std::filesystem::current_path();
	}
} // namespace BHive