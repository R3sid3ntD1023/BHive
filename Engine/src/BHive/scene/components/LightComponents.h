#pragma once

#include "SceneComponent.h"
#include "renderers/Lights.h"
#include "IRenderable.h"

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

		void Serialize(StreamWriter& ar) const;
		void Deserialize(StreamReader& ar);

		REFLECTABLEV(LightComponent)

	};

	struct BHIVE PointLightComponent : public LightComponent
	{
		PointLight mLight{};

		virtual void OnRender(SceneRenderer *renderer);

		void Serialize(StreamWriter& ar) const;
		void Deserialize(StreamReader& ar);

		REFLECTABLEV(LightComponent)
	};

	struct BHIVE DirectionalLightComponent : public LightComponent
	{
		DirectionalLight mLight{};

		virtual void OnRender(SceneRenderer *renderer);

		void Serialize(StreamWriter& ar) const;
		void Deserialize(StreamReader& ar);

		REFLECTABLEV(LightComponent)
	};

	REFLECT_EXTERN(PointLightComponent)
	REFLECT_EXTERN(SpotLightComponent)
	REFLECT_EXTERN(DirectionalLightComponent)

}
