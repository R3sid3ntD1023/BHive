#include "Inspector.h"

namespace BHive
{
	struct Inspector_Type : public Inspector
	{
		virtual bool Inspect(
			const rttr::variant &instance, rttr::variant &var, bool read_only,
			const Inspector::meta_getter &get_meta_data = {}) override;

		REFLECTABLEV(Inspector)
	};

	REFLECT_EXTERN(Inspector_Type)
} // namespace BHive
