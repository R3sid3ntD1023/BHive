#pragma once

#include "Inspector.h"

namespace BHive
{
	struct Inspector_Path : public Inspector
	{
		INSPECTOR_BODY()

		REFLECTABLEV(Inspector)
	};

	REFLECT_EXTERN(Inspector_Path)
} // namespace BHive
