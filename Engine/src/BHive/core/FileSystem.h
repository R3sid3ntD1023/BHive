#pragma once

#include "Core.h"

namespace BHive
{
	struct FileSystem
	{
		static BHIVE bool ReadFile(const std::filesystem::path& filepath, std::string& content);
	};
}