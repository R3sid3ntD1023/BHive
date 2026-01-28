#pragma once

#include "Inspector.h"

namespace BHive
{
	struct InspectorComponent : public Inspector
	{
		INSPECTOR_BODY()

		REFLECTABLEV(Inspector)
	};

	REFLECT_EXTERN(InspectorComponent)
} // namespace BHive