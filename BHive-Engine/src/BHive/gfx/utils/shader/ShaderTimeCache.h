#pragma once

#include "core/Core.h"

namespace BHive
{
	struct ShaderTimeCache
	{
		struct FileTime
		{
			int64_t Last = 0;
			int64_t Modified = 0;

			operator bool() const { return Last == Modified; }
		};

		ShaderTimeCache();
		~ShaderTimeCache();

		bool WasFileModified(const std::filesystem::path &file, FileTime *time);

	private:
		std::unordered_map<std::filesystem::path, FileTime> mTimes;
	};
} // namespace BHive