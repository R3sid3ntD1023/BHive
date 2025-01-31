#pragma once

#include "scene/components/SceneComponent.h"
#include "scene/components/IRenderable.h"

namespace BHive
{
	struct LightComponent : public SceneComponent, public IRenderable
	{
		virtual void OnRender(SceneRenderer *renderer) {};

		REFLECTABLEV(SceneComponent, IRenderable)
	};

	REFLECT(LightComponent)
	{
		BEGIN_REFLECT(LightComponent);
	}

} // namespace BHive