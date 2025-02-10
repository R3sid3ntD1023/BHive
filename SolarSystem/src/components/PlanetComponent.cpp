#include "PlanetComponent.h"
#include "CelestrialBody.h"

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

void PlanetComponent::Save(cereal::JSONOutputArchive &ar) const
{
	ar(MAKE_NVP("Time", mTime));
}

void PlanetComponent::Load(cereal::JSONInputArchive &ar)
{
	ar(MAKE_NVP("Time", mTime));

	if (auto seconds = mTime.ToSeconds(); seconds > 0.f)
		mTheta = 360.f / seconds;
}

REFLECT(PlanetComponent)
{
	BEGIN_REFLECT(PlanetComponent)
	DECLARE_COMPONENT_FUNCS;
}