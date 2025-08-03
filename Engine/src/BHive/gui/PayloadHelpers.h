#pragma once

namespace BHive
{
	struct PayloadHelpers
	{
		static std::string get_entries_buffer(const std::vector<std::filesystem::directory_entry> &entries, size_t &size);

		static std::vector<std::filesystem::directory_entry> get_entries_from_buffer(const char *buffer, size_t size);
	};
} // namespace BHive