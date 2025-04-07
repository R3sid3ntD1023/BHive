#include "Component.h"

namespace BHive
{
	void Component::SetOwner(GameObject *owner)
	{
		mOwningObject = owner;
	}
	void Component::SetTickEnabled(bool enabled)
	{
		mTickEnabled = enabled;
	}
} // namespace BHive
