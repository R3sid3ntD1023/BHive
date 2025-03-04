#include "Component.h"

BEGIN_NAMESPACE(BHive)

void Component::SetOwner(CelestrialBody *owner)
{
	mOwner = owner;
}

void Component::SetTickEnabled(bool enabled)
{
	mTickEnabled = enabled;
}

REFLECT(Component)
{
	BEGIN_REFLECT(Component)
	REFLECT_CONSTRUCTOR();
}

END_NAMESPACE