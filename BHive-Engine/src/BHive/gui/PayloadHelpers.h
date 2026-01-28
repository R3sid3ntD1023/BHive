#pragma once

#include "core/CoreAPI.h"

namespace BHive
{
	struct BHIVE_API PayloadHelpers
	{
		static std::string GetEntriesBuffer(const std::vector<std::filesystem::directory_entry> &entries, size_t &size);

		static std::vector<std::filesystem::directory_entry> GetEntriesFromBuffer(const char *buffer, size_t size);
	};
} // namespace BHive