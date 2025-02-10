#pragma once

#include "ComponentSystem.h"

struct StarComponentSystem : public ComponentSystem
{
	/* data */
	virtual void Update(class Universe *universe, float dt) override;
};
