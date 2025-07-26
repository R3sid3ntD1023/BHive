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

		Ref<Texture2D> Get(const std::filesystem::path &path);

	private:
		std::unordered_map<std::string, Ref<Texture2D>> mCache;
	};
} // namespace BHive
