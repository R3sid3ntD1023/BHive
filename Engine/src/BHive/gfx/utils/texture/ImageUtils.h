#pragma once

#include "core/Core.h"

namespace BHive
{
	class Framebuffer;
	class Texture2D;

	struct BHIVE_API ImageUtils
	{
		static void SaveImage(const std::filesystem::path &path, unsigned w, unsigned h, unsigned c, void *data);
		static void SaveImage(const std::filesystem::path &path, const Ref<Framebuffer> &target, unsigned attachment = 0);
		static void SaveImage(const std::filesystem::path &path, const Ref<Texture2D> &texture);
	};
} // namespace BHive