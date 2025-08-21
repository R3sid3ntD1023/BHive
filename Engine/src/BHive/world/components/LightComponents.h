#pragma once

#include "world/Component.h"
#include "renderers/Lights.h"

namespace BHive
{
	class DirectionalLightComponent : public Component
	{
	public:
		virtual void Save(cereal::BinaryOutputArchive &ar) const override;
		virtual void Load(cereal::BinaryInputArchive &ar) override;

		DirectionalLight Light;

		REFLECTABLEV(Component)
	};

	class PointLightComponent : public Component
	{
	public:
		virtual void Save(cereal::BinaryOutputArchive &ar) const override;
		virtual void Load(cereal::BinaryInputArchive &ar) override;

		PointLight Light;

		REFLECTABLEV(Component)
	};

	class SpotLightComponent : public Component
	{
	public:
		virtual void Save(cereal::BinaryOutputArchive &ar) const override;
		virtual void Load(cereal::BinaryInputArchive &ar) override;

		SpotLight Light;

		REFLECTABLEV(Component)
	};

	REFLECT_EXTERN(DirectionalLightComponent)
	REFLECT_EXTERN(PointLightComponent)
	REFLECT_EXTERN(SpotLightComponent)
} // namespace BHive