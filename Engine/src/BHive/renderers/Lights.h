#pragma once

#include "core/Core.h"
#include "gfx/Color.h"

namespace BHive
{

	struct BHIVE_API LightBase
	{
		virtual ~LightBase() = default;

		FColor Color = 0xFFFFFFFF;

		float Brightness = 1.0f;

		bool operator==(const LightBase &rhs) const;

		template <typename A>
		inline void Serialize(A &ar)
		{
			ar(MAKE_NVP(Color));
		}

		REFLECTABLEV()
	};

	struct BHIVE_API PointLight : public LightBase
	{
		float Radius = 1.0f;

		bool operator==(const PointLight &rhs) const;

		template <typename A>
		inline void Serialize(A &ar)
		{
			ar(cereal::base_class<LightBase>(this), MAKE_NVP(Radius));
		}

		REFLECTABLEV(LightBase)
	};

	struct BHIVE_API SpotLight : public PointLight
	{

		float InnerCutOff = 25.0f;

		float OuterCutOff = 75.0f;

		bool operator==(const SpotLight &rhs) const;

		template <typename A>
		inline void Serialize(A &ar)
		{
			ar(cereal::base_class<PointLight>(this), MAKE_NVP(InnerCutOff), MAKE_NVP(OuterCutOff));
		}

		REFLECTABLEV(PointLight)
	};

	struct BHIVE_API DirectionalLight : public LightBase
	{
		DirectionalLight() = default;
		DirectionalLight(const DirectionalLight &) = default;

		REFLECTABLEV(LightBase)
	};

} // namespace BHive
