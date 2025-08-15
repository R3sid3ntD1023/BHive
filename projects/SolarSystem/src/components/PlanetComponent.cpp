#include "world/GameObject.h"
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
	mOrbitalOrigin = GetOwner()->GetLocalTransform().GetTranslation();
	if (auto seconds = RotationTime.ToSeconds(); seconds > 0.f)
		mTheta = 360.f / seconds;

	if (auto seconds = OrbitalTime.ToSeconds(); seconds > 0.f)
		mOrbitalTheta = 360.f / seconds;
}

void PlanetComponent::Update(float dt)
{
	GetOwner()->GetLocalTransform().AddRotation({0.f, mTheta * dt * 1000.f, 0.f});

	float theta_radians = glm::radians(mOrbitalTheta);

	auto origin = mOrbitalOrigin;
	auto radius = glm::length(origin);
	auto time = BHive::Time::Get();

	float x = glm::cos(theta_radians * time * 100000.f) * radius;
	float y = 0;
	float z = glm::sin(theta_radians * time * 100000.f) * radius;
	GetOwner()->GetLocalTransform().SetTranslation({x, y, z});
}

void PlanetComponent::Save(cereal::BinaryOutputArchive &ar) const
{
	ar(MAKE_NVP(RotationTime), MAKE_NVP(OrbitalTime));
}

void PlanetComponent::Load(cereal::BinaryInputArchive &ar)
{
	ar(MAKE_NVP(RotationTime), MAKE_NVP(OrbitalTime));
}

END_NAMESPACE

RTTR_REGISTRATION
{
	{
		BEGIN_REFLECT(BHive::PlanetTime, "PlanetTime")
		REFLECT_PROPERTY(Years)
		REFLECT_PROPERTY(Days)
		REFLECT_PROPERTY(Hours)
		REFLECT_PROPERTY(Minutes)
		REFLECT_PROPERTY(Seconds);
	}

	{
		BEGIN_REFLECT(BHive::PlanetComponent, "PlanetComponent")(META_DATA(BHive::ClassMetaData_ComponentSpawnable, true)) REFLECT_PROPERTY(RotationTime) REFLECT_PROPERTY(OrbitalTime)
			REFLECT_CONSTRUCTOR() COMPONENT_IMPL();
	}
}