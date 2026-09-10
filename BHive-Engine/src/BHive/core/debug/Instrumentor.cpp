#include "Instrumentor.h"

namespace BHive
{
	Instrumentor &Instrumentor::Get()
	{
		static Instrumentor instance;
		return instance;
	}
} // namespace BHive