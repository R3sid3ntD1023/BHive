#pragma once

#include "renderers/Lights.h"
#include "LightComponent.h"

namespace BHive
{

	struct BHIVE DirectionalLightComponent : public LightComponent
	{
		DirectionalLight mLight{};

		virtual void OnRender(SceneRenderer *renderer);

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;

		virtual void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLEV(LightComponent)
	};

	REFLECT_EXTERN(DirectionalLightComponent)

} // namespace BHive
