#pragma once

#include "core/Core.h"

namespace BHive
{
	class Texture2D;

	class ThumbnailCache
	{
	private:
		/* data */
	public:
		ThumbnailCache();

		const Ref<Texture2D> &Get(const std::filesystem::path &path);

		const Ref<Texture2D> &GetAssetIcon(const std::string &type_name);

	private:
		std::unordered_map<std::string, Ref<Texture2D>> mCache;
		std::unordered_map<std::string, Ref<Texture2D>> mAssetIconCache;
	};
} // namespace BHive
