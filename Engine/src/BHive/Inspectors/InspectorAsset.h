#pragma once

#include "Inspector.h"

namespace BHive
{
	struct Inspector_Asset : public Inspector
	{
		virtual bool Inspect(FPropertyData &property_data, const bool is_read_only) override;

		REFLECTABLEV(Inspector)
	};

	REFLECT_EXTERN(Inspector_Asset)
} // namespace BHive
