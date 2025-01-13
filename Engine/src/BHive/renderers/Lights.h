#pragma once

#include "gfx/Color.h"
#include "reflection/Reflection.h"

#define MAX_LIGHTS 32

namespace BHive
{

	struct Light
	{
		virtual ~Light() = default;

		Color mColor = 0xFFFFFFFF;
		float mBrightness = 1.0f;

		
		REFLECTABLEV()
	};

	struct PointLight : public Light
	{
		float mRadius = 1.0f;

		REFLECTABLEV(Light)
	};


	struct SpotLight : public PointLight
	{
		
		float mInnerCutOff = 25.0f;
		float mOuterCutOff = 75.0f;

		REFLECTABLEV(PointLight)

	};

	struct DirectionalLight : public Light
	{
		DirectionalLight() = default;
		DirectionalLight(const DirectionalLight &) = default;

		REFLECTABLEV(Light)
	};

	
	REFLECT_EXTERN(Light)
	REFLECT_EXTERN(PointLight)
	REFLECT_EXTERN(SpotLight)
	REFLECT_EXTERN(DirectionalLight)
}