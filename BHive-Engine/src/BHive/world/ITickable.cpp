#include "ITickable.h"

namespace BHive
{
	void ITickable::SetTickEnabled(bool enabled)
	{
		mTickEnabled = enabled;
	}

	REFLECT(ITickable)
	{
		BEGIN_REFLECT(ITickable)
		REFLECT_PROPERTY("Tick Enabled", mTickEnabled);
	}
} // namespace BHive