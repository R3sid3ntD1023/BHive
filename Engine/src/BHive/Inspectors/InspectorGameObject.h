#pragma once

#include "Inspector.h"

namespace BHive
{
	struct Inspector_GameObject : public Inspector
	{
		INSPECTOR_BODY()

		REFLECTABLEV(Inspector)
	};

	REFLECT_EXTERN(Inspector_GameObject)
} // namespace BHive