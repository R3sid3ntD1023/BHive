#pragma once

#include "core/Core.h"

namespace BHive
{
	struct FileDialogs
	{
		static BHIVE std::string OpenFile(const char* filter);
		static BHIVE std::string SaveFile(const char* filter);
		static BHIVE std::string GetDirectory();
	};
}