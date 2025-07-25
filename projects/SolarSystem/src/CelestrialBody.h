#pragma once

#include "Component.h"
#include "Universe.h"
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