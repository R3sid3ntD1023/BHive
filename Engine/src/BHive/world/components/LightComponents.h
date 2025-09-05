#pragma once

#include "world/Component.h"
#include "gfx/Color.h"

namespace BHive
{
	class BHIVE_API DirectionalLightComponent : public Component
	{
	public:
		virtual void Save(cereal::BinaryOutputArchive &ar) const override;
		virtual void Load(cereal::BinaryInputArchive &ar) override;

		FColor Color{0xFFFFFFFF};

		REFLECTABLEV(Component)
	};

	class PointLightComponent : public Component
	{
	public:
		virtual void Save(cereal::BinaryOutputArchive &ar) const override;
		virtual void Load(cereal::BinaryInputArchive &ar) override;

		FColor Color{0xFFFFFFFF};

		float Radius{1.f};

		REFLECTABLEV(Component)
	};

	class SpotLightComponent : public Component
	{
	public:
		virtual void Save(cereal::BinaryOutputArchive &ar) const override;
		virtual void Load(cereal::BinaryInputArchive &ar) override;

		FColor Color{0xFFFFFFFF};

		float Radius{1.f};

		float InnerCutoff{45.f};

		float OuterCutoff{75.f};

		REFLECTABLEV(Component)
	};

	REFLECT_EXTERN(DirectionalLightComponent)
	REFLECT_EXTERN(PointLightComponent)
	REFLECT_EXTERN(SpotLightComponent)
} // namespace BHive