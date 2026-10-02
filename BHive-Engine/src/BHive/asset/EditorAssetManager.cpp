#include "EditorAssetManager.h"
#include "Factory.h"
#include "FactoryRegistry.h"

namespace BHive
{

	EditorAssetManager::EditorAssetManager(const std::filesystem::path &directory)
		: mDirectory(directory)
	{

		Deserialize();
	}

	EditorAssetManager::~EditorAssetManager()
	{
		LOG_TRACE("EditorAssetManager Destructor Called");
	}

	Ref<Asset> EditorAssetManager::GetAsset(UUID handle)
	{

		if (!IsAssetHandleValid(handle))
			return nullptr;

		if (mMemoryAssets.contains(handle))
		{
			return mMemoryAssets.at(handle);
		}
		else
		{
			if (IsAssetLoaded(handle))
			{
				return mLoadedAssets.at(handle);
			}
			else
			{
				const FAssetMetaData &metadata = GetMetaData(handle);

				if (auto asset = mAssetSerializer.Import(GetAssetPath(metadata.Name)))
				{
					return mLoadedAssets[handle] = asset;
				}
			}
		}

		LOG_ERROR("Failed to load asset {}", handle.ToString());
		return nullptr;
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

	UUID EditorAssetManager::ImportAsset(const std::filesystem::path &sourcePath)
	{
		auto importPath = sourcePath.is_absolute() ? sourcePath : std::filesystem::relative(sourcePath, GetDirectory());
		auto name = importPath.stem().string();
		auto ext = importPath.extension().string();

		if (auto handle = GetHandle(sourcePath))
		{
			return handle;
		}

		auto factory = mFactoryRegistry.Get(ext);
		if (!factory)
			return NullID;

		auto asset = factory->Import(importPath);
		if (!asset)
			return NullID;

		auto assetPath = GetAssetPath(name);

		if (!mAssetSerializer.Export(asset, assetPath))
			return NullID;

		UUID handle{};

		FAssetMetaData metadata{};
		metadata.Handle = handle;
		metadata.Type = asset->get_type();
		metadata.Name = importPath.stem().string();
		metadata.SourcePath = importPath;
		metadata.SourceTimeStamp = std::filesystem::last_write_time(sourcePath).time_since_epoch().count();

		mAssetRegistry[handle] = metadata;
		mLoadedAssets[handle] = asset;
		mPathToHandle[GetAssetPath(metadata.Name)] = handle;

		Serialize(metadata);

		return handle;
	}

	void EditorAssetManager::ReimportAsset(UUID handle)
	{
		auto &metaData = GetMetaData(handle);
		auto factory = mFactoryRegistry.Get(metaData.Type);
		auto asset = factory->Import(metaData.SourcePath);

		if (mAssetSerializer.Export(asset, GetMetaDataPath(metaData)))
		{
			auto currentTimestamp = std::filesystem::last_write_time(metaData.SourcePath).time_since_epoch().count();
			metaData.SourceTimeStamp = currentTimestamp;
			Serialize(metaData);
		}
	}

	bool EditorAssetManager::RemoveAsset(UUID handle)
	{
		if (mAssetRegistry.contains(handle))
		{
			auto &metaData = GetMetaData(handle);
			auto metaPath = GetMetaDataPath(metaData);
			auto assetPath = GetAssetPath(metaData.Name);

			if (std::filesystem::remove(metaPath) && std::filesystem::remove(assetPath))
			{
				mAssetRegistry.erase(handle);

				return true;
			}
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

		metadata.Name = new_.stem().string();

		Serialize(metadata);

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

		if (mPathToHandle.contains(file); auto handle = mPathToHandle.at(file))
		{
			return mAssetRegistry.at(handle);
		}

		return sNullMetaData;
	}

	FAssetMetaData &EditorAssetManager::GetMetaData(const std::filesystem::path &file)
	{
		static FAssetMetaData sNullMetaData;

		if (mPathToHandle.contains(file); auto handle = mPathToHandle.at(file))
		{
			return mAssetRegistry.at(handle);
		}

		return sNullMetaData;
	}

	UUID EditorAssetManager::GetHandle(const std::filesystem::path &file) const
	{
		if (mPathToHandle.contains(file))
		{
			return mPathToHandle.at(file);
		}

		return NullID;
	}

	const std::filesystem::path &EditorAssetManager::GetFilePath(UUID handle) const
	{
		return GetAssetPath(GetMetaData(handle).Name);
	}

	void EditorAssetManager::Serialize(const FAssetMetaData &metaData) const
	{
		try
		{
			auto directory = GetDirectory();
			if (!std::filesystem::exists(directory))
			{
				std::filesystem::create_directory(directory);
			}

			auto filename = metaData.Name + ".meta";
			std::ofstream out(directory / filename, std::ios::out);
			if (!out)
				return;

			cereal::JSONOutputArchive ar(out);
			ar(metaData);
		}
		catch (std::exception &e)
		{
			LOG_ERROR("EditorAssetManager::Serialize() ERROR: {}", e.what());
		}
	}

	void EditorAssetManager::Deserialize()
	{
		mAssetRegistry.clear();
		mPathToHandle.clear();

		auto directory = GetDirectory();
		if (!std::filesystem::exists(directory))
			return;

		try
		{
			for (auto &entry : std::filesystem::recursive_directory_iterator(directory))
			{
				const auto &path = entry.path();
				const auto &ext = path.extension();

				if (ext != ".meta")
					continue;

				std::ifstream in(path, std::ios::in);
				cereal::JSONInputArchive ar(in);
				FAssetMetaData metaData{};
				ar(metaData);

				mAssetRegistry.emplace(metaData.Handle, metaData);
				mPathToHandle.emplace(metaData.SourcePath, metaData.Handle);
			}

			for (auto &[handle, metaData] : mAssetRegistry)
			{
				if (IsAssetOutofDate(metaData))
				{
					ReimportAsset(handle);
				}
			}
		}
		catch (std::exception &e)
		{
			LOG_ERROR("EditorAssetManager::Deserialize() ERROR: {}", e.what());
		}
	}

	bool EditorAssetManager::IsAssetOutofDate(const FAssetMetaData &metaData) const
	{
		auto timestamp = std::filesystem::last_write_time(metaData.SourcePath).time_since_epoch().count();
		return timestamp > metaData.SourceTimeStamp;
	}

	std::filesystem::path EditorAssetManager::GetDirectory() const
	{
		return !mDirectory.empty() ? mDirectory : std::filesystem::current_path();
	}

	std::filesystem::path EditorAssetManager::GetAssetPath(const std::string &name) const
	{
		return GetDirectory() / (name + ".asset");
	}

	std::filesystem::path BHive::EditorAssetManager::GetMetaDataPath(const FAssetMetaData &metaData) const
	{
		return GetDirectory() / (metaData.Name + ".meta");
	}

} // namespace BHive