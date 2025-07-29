#pragma once

#include "Inspector.h"

namespace BHive
{
	struct Inspector_Asset : public Inspector
	{
		INSPECTOR_BODY()

		REFLECTABLEV(Inspector)
	};

	REFLECT_EXTERN(Inspector_Asset)
} // namespace BHive
