#pragma once

#include "inspectors/Inspector.h"

namespace BHive
{
	struct Inspector_Blackboard : public Inspector
	{
		virtual bool
		Inspect(const rttr::variant &instance, rttr::variant &var, bool read_only, const meta_getter &get_meta_data = {});

		REFLECTABLEV(Inspector)
	};

	REFLECT_EXTERN(Inspector_Blackboard)
} // namespace BHive