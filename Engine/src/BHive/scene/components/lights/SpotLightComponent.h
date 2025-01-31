#pragma once

#include "renderers/Lights.h"
#include "LightComponent.h"

namespace BHive
{
	struct BHIVE SpotLightComponent : public LightComponent
	{
		SpotLight mLight{};

		virtual void OnRender(SceneRenderer *renderer);

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;

		virtual void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLEV(LightComponent)
	};

	REFLECT_EXTERN(SpotLightComponent)

} // namespace BHive
