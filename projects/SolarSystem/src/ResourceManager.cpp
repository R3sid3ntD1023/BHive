#include "ResourceManager.h"
#include <core/serialization/Serialization.h>
#include <importers/TextureImporter.h>
#include <mesh/MeshImporter.h>
#include <mesh/StaticMesh.h>
#include <mesh/primitives/Sphere.h>
#include <asset/FactoryRegistry.h>
#include <renderers/Renderer.h>

namespace BHive
{
	AssetType GetTypeFromExtension(const std::string &ext)
	{
		AssetType type = InvalidType;

		if (ext == ".glb" || ext == ".gltf" || ext == ".obj")
		{
			type = AssetType::get<StaticMesh>();
		}

		else if (ext == ".jpg" || ext == ".png")
		{
			type = AssetType::get<Texture2D>();
		}

		return type;
	}

	ResourceManager::ResourceManager(const std::filesystem::path &directory)
		: mDirectory(directory)
	{

		RegisterImporters();
		LoadRegistry();
		RegisterDefaultAssets();
	}

	bool ResourceManager::Import(const std::filesystem::path &path)
	{
		if (auto type = GetTypeFromExtension(path.extension().string()))
		{
			FAssetMetaData metadata;
			metadata.Name = path.stem().string();
			metadata.Path = std::filesystem::relative(path, mDirectory);
			metadata.Type = type;

			mMetaData.emplace(AssetHandle(), metadata);
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
		auto &importer = mLoadFunctions.at(metadata.Type);
		asset = importer(mDirectory / metadata.Path);

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

		std::ofstream out(mDirectory / "Registry.registry");
		cereal::JSONOutputArchive ar(out);
		ar(mMetaData);
	}

	void ResourceManager::LoadRegistry()
	{
		if (!std::filesystem::exists(mDirectory / "Registry.registry"))
		{
			return;
		}

		std::ifstream in(mDirectory / "Registry.registry");
		cereal::JSONInputArchive ar(in);
		ar(mMetaData);
	}

	void ResourceManager::RegisterImporters()
	{
		auto texture_importer = [](const std::filesystem::path &path) -> Ref<Asset> { return TextureImporter::Import(path); };
		auto mesh_importer = [](const std::filesystem::path &path) -> Ref<Asset>
		{
			FMeshImportData data;
			if (MeshImporter::Import(path, data))
			{
				return CreateRef<StaticMesh>(data.mMeshData);
			}

			return nullptr;
		};

		mLoadFunctions[rttr::type::get<Texture2D>()] = texture_importer;
		mLoadFunctions[rttr::type::get<StaticMesh>()] = mesh_importer;
	}

	void ResourceManager::RegisterDefaultAssets()
	{
		auto grey = 0xFF808080;

		mMemoryAssets[1] = CreateRef<PSphere>(1.f, 32, 32);
		mMemoryAssets[2] = Renderer::GetWhiteTexture();
		mMemoryAssets[3] = Texture2D::Create(&grey, 1, 1, FTextureSpecification{.mChannels = 4, .mFormat = EFormat::RGBA8});
		mMemoryAssets[4] = Renderer::GetBlackTexture();
	}

	bool ResourceManager::IsMemoryAsset(AssetHandle handle) const
	{
		return mMemoryAssets.contains(handle);
	}
} // namespace BHive