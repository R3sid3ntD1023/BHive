#include "PayloadHelpers.h"

namespace BHive
{
	std::string PayloadHelpers::GetEntriesBuffer(const std::vector<std::filesystem::directory_entry> &entries, size_t &size)
	{
		std::string buffer;
		for (const auto &str : entries)
		{
			buffer += str.path().string() + '\0';
			size += buffer.size() + 1;
		}
		return buffer;
	}

	std::vector<std::filesystem::directory_entry> PayloadHelpers::GetEntriesFromBuffer(const char *buffer, size_t size)
	{
		std::vector<std::filesystem::directory_entry> entries;
		const char *current = buffer;
		const char *end = buffer + size;

		while (current < end)
		{
			std::string str(current);
			if (!str.empty())
				entries.emplace_back(str);
			current += str.size() + 1;
		}

		return entries;
	}
} // namespace BHive