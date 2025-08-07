#pragma once

#include "world/Component.h"
#include "world/GameObject.h"

BEGIN_NAMESPACE(BHive)
class Shader;
class Universe;

struct CelestrialBody : public GameObject
{
	CelestrialBody(const entt::entity &handle, World *world);

	REFLECTABLEV()
};

END_NAMESPACE