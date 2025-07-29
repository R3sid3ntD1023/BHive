#pragma once

#include "Inspector.h"

namespace BHive
{
	struct Inspector_SequentialContainer : public Inspector
	{
		INSPECTOR_BODY()

		REFLECTABLEV(Inspector)
	};

	REFLECT_EXTERN(Inspector_SequentialContainer)
} // namespace BHive