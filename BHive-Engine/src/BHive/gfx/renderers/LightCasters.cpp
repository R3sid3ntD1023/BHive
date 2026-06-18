#include "LightCasters.h"
#include "LightCasters.h"

namespace BHive
{

	DirectionalLight & DirectionalLight::SetColor(const FColor &color)
	{
		mColor = color;
		return *this;
	}

	DirectionalLight & DirectionalLight::SetIntensity(float intensity)
	{
		mIntensity = intensity;
		return *this;
	}

	DirectionalLight & DirectionalLight::SetDirection(const glm::vec3 &direction)
	{
		mDirection = direction;
		return *this;
	}

	FGPUDirectionalLight DirectionalLight::ToGPU() const
	{
		FGPUDirectionalLight gpu{};
		gpu.Color = {mColor.r, mColor.g, mColor.b, mIntensity};
		gpu.Direction = {mDirection, 0.0f};
		return gpu;
	}

	PointLight &PointLight::SetColor(const FColor &color)
	{
		mColor = color;
		return *this;
	}

	PointLight &PointLight::SetIntensity(float intensity)
	{
		mIntensity = intensity;
		return *this;
	}

	PointLight &PointLight::SetPosition(const glm::vec3 &position)
	{
		mPosition = position;
		return *this;
	}

	PointLight &PointLight::SetRadius(float radius)
	{
		mRadius = radius;
		return *this;
	}

	FGPUPointLight PointLight::ToGPU() const
	{
		FGPUPointLight gpu{};
		gpu.Color = {mColor.r, mColor.g, mColor.b, mIntensity};
		gpu.Position = {mPosition, mRadius};
		return gpu;
	}

	SpotLight &SpotLight::SetColor(const FColor &color)
	{
		mColor = color;
		return *this;
	}

	SpotLight &SpotLight::SetIntensity(float intensity)
	{
		mIntensity = intensity;
		return *this;
	}

	SpotLight &SpotLight::SetPosition(const glm::vec3 &position)
	{
		mPosition = position;
		return *this;
	}

	SpotLight &SpotLight::SetDirection(const glm::vec3 &direction)
	{
		mDirection = direction;
		return *this;
	}

	SpotLight &SpotLight::SetRadius(float radius)
	{
		mRadius = radius;
		return *this;
	}

	SpotLight &SpotLight::SetInnerAngleDegrees(float degrees)
	{
		mInnerAngleDegrees = degrees;
		mInnerCutOffCos = glm::cos(glm::radians(degrees));
		return *this;
	}

	SpotLight &SpotLight::SetOuterAngleDegrees(float degrees)
	{
		mOuterAngleDegrees = degrees;
		mOuterCutOffCos = glm::cos(glm::radians(degrees));
		return *this;
	}

	FGPUSpotLight SpotLight::ToGPU() const
	{
		FGPUSpotLight gpu{};
		gpu.Color = {mColor.r, mColor.g, mColor.b, mIntensity};
		gpu.Position = {mPosition, mRadius};
		gpu.Direction = {mDirection, mInnerCutOffCos};
		gpu.Params.x = mOuterCutOffCos;
		return gpu;
	}

	REFLECT(DirectionalLight)
	{
		BEGIN_REFLECT(DirectionalLight)
		REFLECT_PROPERTY("Color", GetColor, SetColor)
		REFLECT_PROPERTY("Direction", GetDirection, SetDirection)
		REFLECT_PROPERTY("Intensity", GetIntensity, SetIntensity);
	}

	REFLECT(PointLight)
	{
		BEGIN_REFLECT(PointLight)
		REFLECT_PROPERTY("Color", GetColor, SetColor)
		REFLECT_PROPERTY("Position", GetPosition, SetPosition)
		REFLECT_PROPERTY("Intensity", GetIntensity, SetIntensity)
		REFLECT_PROPERTY("Radius", GetRadius, SetRadius);
	}

	REFLECT(SpotLight)
	{
		BEGIN_REFLECT(SpotLight)
		REFLECT_PROPERTY("Color", GetColor, SetColor)
		REFLECT_PROPERTY("Direction", GetDirection, SetDirection)
		REFLECT_PROPERTY("Position", GetPosition, SetPosition)
		REFLECT_PROPERTY("Intensity", GetIntensity, SetIntensity)
		REFLECT_PROPERTY("Radius", GetRadius, SetRadius)
		REFLECT_PROPERTY("Inner Angle", GetInnerAngleDegrees, SetInnerAngleDegrees)
		REFLECT_PROPERTY("Outer Angle", GetOuterAngleDegrees, SetOuterAngleDegrees);
	}
} // namespace BHive

