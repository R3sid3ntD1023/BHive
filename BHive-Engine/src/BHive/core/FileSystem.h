#pragma once

#include "Core.h"

namespace BHive
{
	struct FileSystem
	{
		static bool ReadFile(const std::filesystem::path &filepath, std::string &content);

		static bool ReadFile(const std::filesystem::path &filepath, std::vector<uint32_t> &data);

		static bool WriteFile(const std::filesystem::path &filepath, const std::vector<uint32_t> &data);

		static std::filesystem::path ResolvePath(const std::filesystem::path &path, const std::filesystem::path &basePath = std::filesystem::current_path());
	};
} // namespace BHive