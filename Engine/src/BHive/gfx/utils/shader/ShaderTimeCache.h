#pragma once

#include "core/Core.h"

namespace BHive
{
	struct ShaderTimeCache
	{
		ShaderTimeCache();
		~ShaderTimeCache();

		bool WasFileModified(const std::filesystem::path &file);

	private:
		std::unordered_map<std::filesystem::path, long long> mModifiedTimes;
		std::unordered_map<std::filesystem::path, long long> mLastTimes;
	};
} // namespace BHive