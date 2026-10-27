#pragma once

#include "Inspector.h"

namespace BHive
{
	struct Inspector_AssociativeContainer : public Inspector
	{
		INSPECTOR_BODY()

		REFLECTABLEV(Inspector)
	};

	REFLECT_EXTERN(Inspector_AssociativeContainer)
} // namespace BHive
