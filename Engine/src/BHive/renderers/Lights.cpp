#include "Lights.h"

namespace BHive
{
	bool Light::operator==(const Light &rhs) const
	{
		return rhs.mBrightness == mBrightness && rhs.mColor == mColor;
	}

	bool PointLight::operator==(const PointLight &rhs) const
	{
		return rhs.mRadius == mRadius;
	}

	bool SpotLight::operator==(const SpotLight &rhs) const
	{
		return rhs.mInnerCutOff == mInnerCutOff && mOuterCutOff == rhs.mOuterCutOff;
	}

	ELightType PointLight::GetLightType() const
	{
		return ELightType::Point;
	}

	ELightType SpotLight::GetLightType() const
	{
		return ELightType::SpotLight;
	}

	ELightType DirectionalLight::GetLightType() const
	{
		return ELightType::Directional;
	}
} // namespace BHive