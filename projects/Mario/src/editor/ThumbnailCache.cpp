#include "ThumbnailCache.h"
#include "importers/TextureImporter.h"

namespace BHive
{

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