#include "CelestrialBody.h"
#include "PlanetComponent.h"
#include <core/Time.h>

BEGIN_NAMESPACE(BHive)

uint32_t PlanetTime::ToSeconds()
{
	uint32_t seconds = 0;

	seconds = Years * 31'536'000;
	seconds += Days * 86'400;
	seconds += Hours * 3'600;
	seconds += Minutes * 60;
	seconds += Seconds;

	return seconds;
}

void PlanetComponent::Begin()
{
	mOrbitalOrigin = GetOwner()->GetLocalTransform().get_translation();
	if (auto seconds = RotationTime.ToSeconds(); seconds > 0.f)
		mTheta = 360.f / seconds;

	if (auto seconds = OrbitalTime.ToSeconds(); seconds > 0.f)
		mOrbitalTheta = 360.f / seconds;
}

void PlanetComponent::Update(float dt)
{
	GetOwner()->GetLocalTransform().add_rotation({0.f, mTheta * dt * 1000.f, 0.f});

	float theta_radians = glm::radians(mOrbitalTheta);

	auto origin = mOrbitalOrigin;
	auto radius = glm::length(origin);
	auto time = BHive::Time::Get();

	float x = glm::cos(theta_radians * time * 100000.f) * radius;
	float y = 0;
	float z = glm::sin(theta_radians * time * 100000.f) * radius;
	GetOwner()->GetLocalTransform().set_translation({x, y, z});
}

void PlanetComponent::Save(cereal::JSONOutputArchive &ar) const
{
	ar(MAKE_NVP(RotationTime), MAKE_NVP(OrbitalTime));
}

void PlanetComponent::Load(cereal::JSONInputArchive &ar)
{
	ar(MAKE_NVP(RotationTime), MAKE_NVP(OrbitalTime));
}

REFLECT(PlanetComponent)
{
	BEGIN_REFLECT(PlanetComponent)
	REFLECT_CONSTRUCTOR() COMPONENT_IMPL();
}

END_NAMESPACE