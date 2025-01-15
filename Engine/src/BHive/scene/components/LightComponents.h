#pragma once

#include "IRenderable.h"
#include "renderers/Lights.h"
#include "SceneComponent.h"

namespace BHive
{
	struct LightComponent : public SceneComponent, public IRenderable
	{
		virtual void OnRender(SceneRenderer *renderer) {};

		REFLECTABLEV(SceneComponent, IRenderable)
	};

	struct BHIVE SpotLightComponent : public LightComponent
	{
		SpotLight mLight{};

		virtual void OnRender(SceneRenderer *renderer);

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;

		virtual void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLEV(LightComponent)
	};

	struct BHIVE PointLightComponent : public LightComponent
	{
		PointLight mLight{};

		virtual void OnRender(SceneRenderer *renderer);

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;

		virtual void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLEV(LightComponent)
	};

	struct BHIVE DirectionalLightComponent : public LightComponent
	{
		DirectionalLight mLight{};

		virtual void OnRender(SceneRenderer *renderer);

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;

		virtual void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLEV(LightComponent)
	};

	REFLECT_EXTERN(PointLightComponent)
	REFLECT_EXTERN(SpotLightComponent)
	REFLECT_EXTERN(DirectionalLightComponent)

} // namespace BHive
