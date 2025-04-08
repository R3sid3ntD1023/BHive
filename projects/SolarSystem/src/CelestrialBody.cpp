#include "CelestrialBody.h"
#include "Component.h"
#include <core/serialization/Serialization.h>

BEGIN_NAMESPACE(BHive)

CelestrialBody::CelestrialBody(const entt::entity &handle, World *world)
	: GameObject(handle, world)
{
}

REFLECT(CelestrialBody)
{
	BEGIN_REFLECT(CelestrialBody)
	REFLECT_CONSTRUCTOR(const entt::entity &, World *);
}

END_NAMESPACE