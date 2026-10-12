#pragma once

#include "core/Core.h"
#include "gfx/font/Font.h"

namespace BHive
{
	struct BHIVE_API FontFactory
	{
		static Ref<class Font> Create(const std::filesystem::path &path, uint32_t fontSize);
	};
} // namespace BHive
