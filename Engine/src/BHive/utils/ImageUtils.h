#pragma once

#include "core/Core.h"

namespace BHive
{
	class Framebuffer;

	struct ImageUtils
	{
		static void SaveImage(const std::filesystem::path &path, unsigned w, unsigned h, unsigned c, void *data);
		static void SaveImage(const std::filesystem::path &path, const Ref<Framebuffer> &target, unsigned attachment = 0);
	};
} // namespace BHive