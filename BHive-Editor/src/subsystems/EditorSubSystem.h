#pragma once

#include "ThumbnailCache.h"

namespace BHive
{
	class Texture2D;

	struct EditorSubSystem
	{
		Ref<Texture2D> GetIcon(const std::filesystem::path &path);

	private:
		ThumbnailCache mIconCache;
	};
} // namespace BHive
