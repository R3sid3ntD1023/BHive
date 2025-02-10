#include "PlanetComponentSystem.h"
#include "components/PlanetComponent.h"
#include "components/IDComponent.h"
#include "Universe.h"
#include "CelestrialBody.h"

void PlanetComponentSystem::Update(Universe *universe, float dt)
{
	auto view = universe->GetRegistry().view<IDComponent, PlanetComponent>();
	for (auto &e : view)
	{
		auto [idcomponent, planetcomponent] = view.get(e);
		auto body = universe->GetBody(idcomponent.mID);
		body->GetLocalTransform().add_rotation({0.f, planetcomponent.mTheta * dt * 1000.f, 0.f});
	}
}