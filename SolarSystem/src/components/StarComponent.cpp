#include "StarComponent.h"
#include "CelestrialBody.h"

void StarComponent::Save(cereal::JSONOutputArchive &ar) const
{
	ar(MAKE_NVP("Brightness", mBrightness), MAKE_NVP("Radius", mRadius), MAKE_NVP("Color", mColor));
}

void StarComponent::Load(cereal::JSONInputArchive &ar)
{
	ar(MAKE_NVP("Brightness", mBrightness), MAKE_NVP("Radius", mRadius), MAKE_NVP("Color", mColor));
}

REFLECT(StarComponent)
{
	BEGIN_REFLECT(StarComponent)
	DECLARE_COMPONENT_FUNCS;
}