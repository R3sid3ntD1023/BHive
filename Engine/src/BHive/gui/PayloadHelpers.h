#pragma once

namespace BHive
{
	struct PayloadHelpers
	{
		static std::string GetEntriesBuffer(const std::vector<std::filesystem::directory_entry> &entries, size_t &size);

		static std::vector<std::filesystem::directory_entry> GetEntriesFromBuffer(const char *buffer, size_t size);
	};
} // namespace BHive