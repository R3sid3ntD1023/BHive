#pragma once

#include "Inspector.h"

namespace BHive
{
	struct Inspector_GameObject : public Inspector
	{
		virtual bool Inspect(
			const rttr::variant &instance, rttr::variant &var, bool read_only,
			const Inspector::meta_getter &get_meta_data = {}) override;

		REFLECTABLEV(Inspector)
	};
} // namespace BHive