#pragma once

#include "world/Component.h"
#include "renderers/Lights.h"

namespace BHive
{
	class DirectionalLightComponent : public Component
	{
	public:
		virtual void Render() override;

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;
		virtual void Load(cereal::BinaryInputArchive &ar) override;

	private:
		REFLECTABLEV(Component)

		DirectionalLight mLight;
	};

	class PointLightComponent : public Component
	{
	public:
		virtual void Render() override;

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;
		virtual void Load(cereal::BinaryInputArchive &ar) override;

	private:
		REFLECTABLEV(Component)

		PointLight mLight;
	};

	class SpotLightComponent : public Component
	{
	public:
		virtual void Render() override;
		virtual void Save(cereal::BinaryOutputArchive &ar) const override;
		virtual void Load(cereal::BinaryInputArchive &ar) override;

	private:
		REFLECTABLEV(Component)
		SpotLight mLight;
	};

	REFLECT_EXTERN(DirectionalLightComponent)
	REFLECT_EXTERN(PointLightComponent)
	REFLECT_EXTERN(SpotLightComponent)
} // namespace BHive