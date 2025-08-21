#include "Lights.h"

namespace BHive
{
	bool LightBase::operator==(const LightBase &rhs) const
	{
		return rhs.Color == Color;
	}

	bool PointLight::operator==(const PointLight &rhs) const
	{
		return rhs.Radius == Radius;
	}

	bool SpotLight::operator==(const SpotLight &rhs) const
	{
		return rhs.InnerCutOff == InnerCutOff && OuterCutOff == rhs.OuterCutOff;
	}

} // namespace BHive