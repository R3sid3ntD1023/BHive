#include "ThumbnailCache.h"
#include "asset/EditorAssetManager.h"
#include "gfx/factories/TextureFactory.h"

namespace BHive
{

	ThumbnailCache::ThumbnailCache()
	{
		mCache["FolderIcon"] = TextureImporter::FromFile(EDITOR_RESOURCE_PATH "icons/non_empty_folder.png");
		mCache["FolderIconEmpty"] = TextureImporter::FromFile(EDITOR_RESOURCE_PATH "icons/empty_folder.png");
		mCache["Invalid"] = TextureImporter::FromFile(EDITOR_RESOURCE_PATH "icons/invalid.png");
		mCache["PlayIcon"] = TextureImporter::FromFile(EDITOR_RESOURCE_PATH "icons/PlayButton.png");
		mCache["PauseIcon"] = TextureImporter::FromFile(EDITOR_RESOURCE_PATH "icons/PauseButton.png");
		mCache["StepIcon"] = TextureImporter::FromFile(EDITOR_RESOURCE_PATH "icons/StepButton.png");
		mCache["StopIcon"] = TextureImporter::FromFile(EDITOR_RESOURCE_PATH "icons/StopButton.png");

		// asset icons
		mAssetIconCache["BDRFMaterial"] = TextureImporter::FromFile(EDITOR_RESOURCE_PATH "icons/material.png");
		mAssetIconCache["StaticMesh"] = TextureImporter::FromFile(EDITOR_RESOURCE_PATH "icons/static_mesh.png");
		mAssetIconCache["SkeletalMesh"] = TextureImporter::FromFile(EDITOR_RESOURCE_PATH "icons/skeletal_mesh.png");
		mAssetIconCache["Skeleton"] = TextureImporter::FromFile(EDITOR_RESOURCE_PATH "icons/skeleton.png");
		mAssetIconCache["SkeletalAnimation"] = TextureImporter::FromFile(EDITOR_RESOURCE_PATH "icons/animation.png");
		mAssetIconCache["Texture2D"] = TextureImporter::FromFile(EDITOR_RESOURCE_PATH "icons/texture_2d.png");
		mAssetIconCache["World"] = TextureImporter::FromFile(EDITOR_RESOURCE_PATH "icons/world.png");
		mAssetIconCache["AnimGraph"] = TextureImporter::FromFile(EDITOR_RESOURCE_PATH "icons/graph.png");

		mAssetIconCache["PointLight"] = TextureImporter::FromFile(EDITOR_RESOURCE_PATH "icons/pointlight.png");
	}

	TexturePtr ThumbnailCache::Get(const std::filesystem::path &path)
	{
		auto name = path.filename().string();
		auto hash = std::hash<std::string>{}(name);

		if (mLoadedIcons.contains(hash))
			return mLoadedIcons.at(hash);

		auto texture = TextureImporter::FromFile(path);
		TextureImporter::Resize(texture, {64, 64});
		mCache.emplace(name, texture);

		mLoadedIcons.emplace(hash, TextureFactory::Create2D(texture));

		return mLoadedIcons.at(hash);
	}

	TexturePtr ThumbnailCache::GetAssetIcon(const std::string &type_name)
	{
		auto hash = std::hash<std::string>{}(type_name);

		if (mLoadedIcons.contains(hash))
			return mLoadedIcons.at(hash);
		mLoadedIcons.emplace(hash, TextureFactory::Create2D(mAssetIconCache[type_name]));

		return mLoadedIcons.at(hash);
	}
} // namespace BHive