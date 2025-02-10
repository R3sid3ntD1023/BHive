#include "StarComponentSystem.h"
#include "components/StarComponent.h"
#include "components/IDComponent.h"
#include "Universe.h"
#include "CelestrialBody.h"
#include <renderers/Renderer.h>

void StarComponentSystem::Update(Universe *universe, float dt)
{
	auto view = universe->GetRegistry().view<IDComponent, StarComponent>();
	for (auto &e : view)
	{
		auto [idcomponent, starcomponent] = view.get(e);

		auto world_transform = universe->GetBody(idcomponent.mID)->GetTransform();

		BHive::PointLight light;
		light.mBrightness = starcomponent.mBrightness;
		light.mRadius = starcomponent.mRadius;
		light.mColor = starcomponent.mColor;
		BHive::Renderer::SubmitPointLight(world_transform.get_translation(), light);
	}
}