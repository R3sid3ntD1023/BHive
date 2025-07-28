#include "Inspector.h"

namespace BHive
{
	struct Inspector_Type : public Inspector
	{
		virtual bool Inspect(FPropertyData &property_data, const bool is_read_only) override;

		REFLECTABLEV(Inspector)
	};

	REFLECT_EXTERN(Inspector_Type)
} // namespace BHive
