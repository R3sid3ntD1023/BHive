#pragma once

#include "core/Core.h"

namespace BHive
{
	struct BHIVE_API FileDialogs
	{
		static std::string OpenFile(const char *filter);
		static std::string SaveFile(const char *filter);
		static std::string GetDirectory();
		static bool MoveToRecycleBin(const std::string &path);
	};
} // namespace BHive