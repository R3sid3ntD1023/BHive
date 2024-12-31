#include "FileSystem.h"
#include <fstream>

namespace BHive
{
	bool FileSystem::ReadFile(const std::filesystem::path& filepath, std::string& content)
	{
		std::ifstream in(filepath.string().c_str(), std::ios::in | std::ios::ate | std::ios::binary);

		if (!in)
		{
			LOG_ERROR("Failed to open file {}", filepath.string());
			return false;
		}

		auto end = in.tellg();
		in.seekg(0, std::ios::beg);
		auto size = end - in.tellg();

		if (size <= 0)
		{
			LOG_ERROR("Failed to read file {}", filepath.string());
			return false;
		}

		content.resize(size);
		in.read(&content[0], size);

		return true;
	}

}