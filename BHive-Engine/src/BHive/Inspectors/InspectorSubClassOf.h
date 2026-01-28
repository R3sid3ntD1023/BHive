#pragma once

#include "Inspector.h"

namespace BHive
{
	struct Inspector_SubClassOf : public Inspector
	{
		INSPECTOR_BODY()

		REFLECTABLEV(Inspector)
	};

	REFLECT_EXTERN(Inspector_SubClassOf)
} // namespace BHive