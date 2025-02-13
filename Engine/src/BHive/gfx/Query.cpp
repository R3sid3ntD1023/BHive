#include "Query.h"
#include "Platform/GL/GLQuery.h"

namespace BHive
{
	Ref<QueryTimer> QueryTimer::Create()
	{
		return CreateRef<GLQueryTimer>();
	}
} // namespace BHive