#include "Instrumentor.h"

namespace BHive
{
    static Instrumentor & Instrumentor::Get()
		{
			static Instrumentor instance;
			return instance;
		}
}