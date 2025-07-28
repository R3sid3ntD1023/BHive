#pragma once

#include "Inspector.h"

namespace BHive
{
	struct Inspector_GameObject : public Inspector
	{
		virtual bool Inspect(FPropertyData &data, const bool is_read_only) override;

		REFLECTABLEV(Inspector)
	};

	REFLECT_EXTERN(Inspector_GameObject)
} // namespace BHive