#include "ThumbnailCache.h"
#include "importers/TextureImporter.h"
#include "asset/EditorAssetManager.h"

namespace BHive
{

	ThumbnailCache::ThumbnailCache()
	{
		mCache["FolderIcon"] = TextureLoader::Import(EDITOR_RESOURCE_PATH "icons/non_empty_folder.png");
		mCache["FolderIconEmpty"] = TextureLoader::Import(EDITOR_RESOURCE_PATH "icons/empty_folder.png");
		mCache["Invalid"] = TextureLoader::Import(EDITOR_RESOURCE_PATH "icons/invalid.png");
		mCache["PlayIcon"] = TextureLoader::Import(EDITOR_RESOURCE_PATH "icons/PlayButton.png");
		mCache["PauseIcon"] = TextureLoader::Import(EDITOR_RESOURCE_PATH "icons/PauseButton.png");
		mCache["StepIcon"] = TextureLoader::Import(EDITOR_RESOURCE_PATH "icons/StepButton.png");
		mCache["StopIcon"] = TextureLoader::Import(EDITOR_RESOURCE_PATH "icons/StopButton.png");

		// asset icons
		mAssetIconCache["BDRFMaterial"] = TextureLoader::Import(EDITOR_RESOURCE_PATH "icons/material.png");
		mAssetIconCache["StaticMesh"] = TextureLoader::Import(EDITOR_RESOURCE_PATH "icons/static_mesh.png");
		mAssetIconCache["SkeletalMesh"] = TextureLoader::Import(EDITOR_RESOURCE_PATH "icons/skeletal_mesh.png");
		mAssetIconCache["Skeleton"] = TextureLoader::Import(EDITOR_RESOURCE_PATH "icons/skeleton.png");
		mAssetIconCache["SkeletalAnimation"] = TextureLoader::Import(EDITOR_RESOURCE_PATH "icons/animation.png");
		mAssetIconCache["Texture2D"] = TextureLoader::Import(EDITOR_RESOURCE_PATH "icons/texture_2d.png");
		mAssetIconCache["World"] = TextureLoader::Import(EDITOR_RESOURCE_PATH "icons/world.png");
		mAssetIconCache["AnimGraph"] = TextureLoader::Import(EDITOR_RESOURCE_PATH "icons/graph.png");

		mAssetIconCache["PointLight"] = TextureLoader::Import(EDITOR_RESOURCE_PATH "icons/pointlight.png");
	}

	const Ref<Texture2D> &ThumbnailCache::Get(const std::filesystem::path &path)
	{
		auto name = path.filename().string();

		if (mCache.contains(name))
			return mCache.at(name);

		auto texture = TextureLoader::Import(path, {.mWidth = 64, .mHeight = 64});
		mCache.emplace(name, texture);

		return mCache.at(name);
	}

	const Ref<Texture2D> &ThumbnailCache::GetAssetIcon(const std::string &type_name)
	{
		return mAssetIconCache[type_name];
	}
} // namespace BHive