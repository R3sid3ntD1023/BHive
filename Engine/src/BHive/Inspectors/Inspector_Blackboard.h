#pragma once

#include "inspectors/Inspector.h"

namespace BHive
{
	struct Inspector_Blackboard : public Inspector
	{
		virtual bool Inspect(FPropertyData &property_data, const bool is_read_only);

		REFLECTABLEV(Inspector)
	};

	REFLECT_EXTERN(Inspector_Blackboard)
} // namespace BHive