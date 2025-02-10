#include "ComponentSystem.h"

struct PlanetComponentSystem : public ComponentSystem
{
	/* data */
	virtual void Update(class Universe *universe, float dt) override;
};
