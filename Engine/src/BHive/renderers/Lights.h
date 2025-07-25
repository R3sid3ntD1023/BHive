#pragma once

#include "core/Core.h"
#include "gfx/Color.h"

#define MAX_LIGHTS 32

namespace BHive
{
	enum class ELightType : uint32_t
	{
		Directional = 0,
		Point = 1,
		SpotLight = 2
	};

	struct BHIVE_API Light
	{
		virtual ~Light() = default;

		FColor mColor = 0xFFFFFFFF;

		float mBrightness = 1.0f;

		virtual ELightType GetLightType() const = 0;

		template <typename A>
		inline void Serialize(A &ar)
		{
			ar(mColor, mBrightness);
		}

		REFLECTABLEV()
	};

	struct BHIVE_API PointLight : public Light
	{
		float mRadius = 1.0f;

		virtual ELightType GetLightType() const override { return ELightType::Point; }

		template <typename A>
		inline void Serialize(A &ar)
		{
			Light::Serialize(ar);
			ar(mRadius);
		}

		REFLECTABLEV(Light)
	};

	struct BHIVE_API SpotLight : public PointLight
	{

		float mInnerCutOff = 25.0f;
		float mOuterCutOff = 75.0f;

		virtual ELightType GetLightType() const override { return ELightType::SpotLight; }

		template <typename A>
		inline void Serialize(A &ar)
		{
			PointLight::Serialize(ar);
			ar(mInnerCutOff, mOuterCutOff);
		}

		REFLECTABLEV(PointLight)
	};

	struct BHIVE_API DirectionalLight : public Light
	{
		DirectionalLight() = default;
		DirectionalLight(const DirectionalLight &) = default;

		virtual ELightType GetLightType() const override { return ELightType::Directional; }

		REFLECTABLEV(Light)
	};

} // namespace BHive