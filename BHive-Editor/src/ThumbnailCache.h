#pragma once

#include "core/Core.h"
#include "gfx/registries/Handles.h"
#include "importers/TextureImporter.h"

namespace BHive
{
	class ThumbnailCache
	{
	private:
		/* data */
	public:
		ThumbnailCache();

		TexturePtr Get(const std::filesystem::path &path);

		TexturePtr GetAssetIcon(const std::string &type_name);

	private:
		std::unordered_map<std::string, DecodedTexture> mCache;
		std::unordered_map<std::string, DecodedTexture> mAssetIconCache;
		std::unordered_map<uint64_t, TexturePtr> mLoadedIcons;
	};
} // namespace BHive
