#pragma once

#include "core/Core.h"

namespace BHive
{
	struct BHIVE_API GUI
	{
		static void BeginDockSpace(const char *strid, bool *opened = (bool *)0, float title_height = 0);
		static void EndDockSpace();
		static const char *GetDockSpaceID() { return sDockSpaceID; }

	private:
		static void ShowDockingDisabledMessage();
		static inline const char *sDockSpaceID = "";
	};
} // namespace BHive