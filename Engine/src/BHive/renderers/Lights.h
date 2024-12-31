#pragma once

#include "gfx/Color.h"
#include "reflection/Reflection.h"
#include "serialization/Serialization.h"

#define MAX_LIGHTS 32

namespace BHive
{

	struct Light
	{
		Light() = default;
		Light(const Light &) = default;
		virtual ~Light() = default;

		Color mColor = 0xFFFFFFFF;
		float mBrightness = 1.0f;

		virtual void Serialize(StreamWriter &ar) const;
		virtual void Deserialize(StreamReader &ar);

		REFLECTABLEV()
	};

	struct SpotLight : public Light
	{
		SpotLight() = default;
		SpotLight(const SpotLight &) = default;

		float mRadius = 1.0f;
		float mInnerCutOff = 25.0f;
		float mOuterCutOff = 75.0f;

		REFLECTABLEV(Light)

		void Serialize(StreamWriter &ar) const;
		void Deserialize(StreamReader &ar);
	};

	struct DirectionalLight : public Light
	{
		DirectionalLight() = default;
		DirectionalLight(const DirectionalLight &) = default;

		REFLECTABLEV(Light)
	};

	struct PointLight : public Light
	{
		PointLight() = default;
		PointLight(const PointLight &) = default;

		float mRadius = 1.0f;

		void Serialize(StreamWriter &ar) const;
		void Deserialize(StreamReader &ar);

		REFLECTABLEV(Light)
	};

	REFLECT_EXTERN(Light)
	REFLECT_EXTERN(PointLight)
	REFLECT_EXTERN(SpotLight)
	REFLECT_EXTERN(DirectionalLight)
}