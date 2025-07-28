#pragma once

#include "Inspector.h"

namespace BHive
{
	struct Inspector_Path : public Inspector
	{
		virtual bool Inspect(FPropertyData &property_data, const bool is_read_only) override;

		REFLECTABLEV(Inspector)
	};

	REFLECT_EXTERN(Inspector_Path)
} // namespace BHive
