#pragma once

#include "Inspector.h"

namespace BHive
{
	struct Inspector_SubClassOf : public Inspector
	{
		virtual bool Inspect(rttr::variant &var, bool read_only, const meta_getter &get_meta_data) override;

		REFLECTABLEV(Inspector)
	};
} // namespace BHive