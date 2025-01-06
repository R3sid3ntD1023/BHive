#pragma once

#include <filesystem>

namespace BHive
{
	struct ImageUtils
	{
		static void SaveImage(const std::filesystem::path& path, unsigned w, unsigned h, unsigned c, void* data);
	};
}