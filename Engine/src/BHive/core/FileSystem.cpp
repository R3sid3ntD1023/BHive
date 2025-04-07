#include "FileSystem.h"
#include <fstream>

namespace BHive
{
	bool FileSystem::ReadFile(const std::filesystem::path &filepath, std::string &content)
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
		in.read(content.data(), size);

		return true;
	}

	bool FileSystem::ReadFile(const std::filesystem::path &filepath, std::vector<uint32_t> &data)
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

		data.resize(size / sizeof(uint32_t));
		in.read((char *)data.data(), size);

		return true;
	}

	bool FileSystem::WriteFile(const std::filesystem::path &filepath, const std::vector<uint32_t> &data)
	{
		std::ofstream out(filepath.string().c_str(), std::ios::out | std::ios::binary);
		if (!out)
		{
			LOG_ERROR("Failed to open file {}", filepath.string());
			return false;
		}
		out.write((const char *)data.data(), data.size() * sizeof(uint32_t));
		return true;
	}

} // namespace BHive