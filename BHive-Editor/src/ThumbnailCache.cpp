#include "ThumbnailCache.h"
#include "importers/TextureImporter.h"
#include "asset/EditorAssetManager.h"

namespace BHive
{

	ThumbnailCache::ThumbnailCache()
	{
		mCache["DirectoryIcon"] = TextureLoader::Import(EDITOR_RESOURCE_PATH "icons/DirectoryIcon.png");
		mCache["FileIcon"] = TextureLoader::Import(EDITOR_RESOURCE_PATH "icons/FileIcon.png");
		mCache["PlayIcon"] = TextureLoader::Import(EDITOR_RESOURCE_PATH "icons/PlayButton.png");
		mCache["PauseIcon"] = TextureLoader::Import(EDITOR_RESOURCE_PATH "icons/PauseButton.png");
		mCache["StepIcon"] = TextureLoader::Import(EDITOR_RESOURCE_PATH "icons/StepButton.png");
		mCache["StopIcon"] = TextureLoader::Import(EDITOR_RESOURCE_PATH "icons/StopButton.png");
	}

	Ref<Texture2D> ThumbnailCache::Get(const std::filesystem::path &path)
	{
		auto name = path.filename().string();

		if (mCache.contains(name))
			return mCache.at(name);

		auto texture = TextureLoader::Import(path, {.mWidth = 128, .mHeight = 128});
		mCache.emplace(name, texture);

		return mCache.at(name);
	}
} // namespace BHive