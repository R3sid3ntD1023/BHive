#pragma once

#include "renderers/Lights.h"
#include "LightComponent.h"

namespace BHive
{
	struct BHIVE PointLightComponent : public LightComponent
	{
		PointLight mLight{};

		virtual void OnRender(SceneRenderer *renderer);

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;

		virtual void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLEV(LightComponent)

	private:
		static Ref<class Texture> GetPointLightIcon();
	};

	REFLECT_EXTERN(PointLightComponent)

} // namespace BHive
