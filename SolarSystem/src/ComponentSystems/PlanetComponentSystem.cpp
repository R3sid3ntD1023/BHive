#include "PlanetComponentSystem.h"
#include "components/PlanetComponent.h"
#include "components/IDComponent.h"
#include "Universe.h"
#include "CelestrialBody.h"
#include "core/profiler/CPUGPUProfiler.h"

std::unordered_map<BHive::UUID, glm::vec3> mOrigins;

void PlanetComponentSystem::Update(Universe *universe, float dt)
{
	CPU_PROFILER_FUNCTION();

	mTime += dt;

	auto view = universe->GetRegistry().view<IDComponent, PlanetComponent>();
	for (auto &e : view)
	{
		auto [idcomponent, planetcomponent] = view.get(e);
		auto body = universe->GetBody(idcomponent.mID);
		body->GetLocalTransform().add_rotation({0.f, planetcomponent.mTheta * dt * 1000.f, 0.f});
	}

	{
		auto view = universe->GetRegistry().view<IDComponent, RevolutionComponent>();
		for (auto &e : view)
		{
			auto [idcomponent, revolutioncomponent] = view.get(e);

			auto body = universe->GetBody(idcomponent.mID);
			float theta_radians = glm::radians(revolutioncomponent.mRevolutionTheta);

			if (!mOrigins.contains(idcomponent.mID))
			{
				mOrigins.emplace(idcomponent.mID, body->GetLocalTransform().get_translation());
			}

			auto origin = mOrigins.at(idcomponent.mID);
			auto radius = glm::length(origin);

			float x = glm::cos(theta_radians * mTime * 10000.f) * radius;
			float y = 0;
			float z = glm::sin(theta_radians * mTime * 10000.f) * radius;
			body->GetLocalTransform().set_translation({x, y, z});

			// LOG_TRACE(" time-{}, x-{}, z-{}", mTime, x, z);
		}
	}
}