#include "Inspector.h"

namespace BHive
{
	struct Inspector_Type : public Inspector
	{
		INSPECTOR_BODY()

		REFLECTABLEV(Inspector)
	};

	REFLECT_EXTERN(Inspector_Type)
} // namespace BHive
