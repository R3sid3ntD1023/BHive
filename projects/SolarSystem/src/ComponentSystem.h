#pragma once

#include <core/Core.h>

struct ComponentSystem
{
	virtual void Update(class Universe *universe, float dt) = 0;
};