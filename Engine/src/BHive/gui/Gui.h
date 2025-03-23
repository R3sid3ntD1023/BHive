#pragma once

#include "core/Core.h"

namespace BHive
{
	struct GUI
	{
		static BHIVE void BeginDockSpace(const char *strid, bool *opened = (bool *)0, float title_height = 0);
		static BHIVE void EndDockSpace();

	private:
		static BHIVE void ShowDockingDisabledMessage();
	};
} // namespace BHive