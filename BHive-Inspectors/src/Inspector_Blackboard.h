#pragma once

#include "Inspector.h"

namespace BHive
{
	struct Inspector_Blackboard : public Inspector
	{
		INSPECTOR_BODY()

		REFLECTABLEV(Inspector)
	};

	REFLECT_EXTERN(Inspector_Blackboard)
} // namespace BHive