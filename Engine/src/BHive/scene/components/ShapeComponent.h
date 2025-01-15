#pragma once

#include "math/AABB.hpp"
#include "scene/components/IRenderable.h"
#include "SceneComponent.h"

namespace BHive
{
	class SceneRenderer;

	struct BHIVE ShapeComponent : public SceneComponent, public IRenderable
	{
		virtual AABB GetBoundingBox() const = 0;

		virtual void OnRender(SceneRenderer *renderer) {};

		REFLECTABLEV(SceneComponent, IRenderable)
	};

	REFLECT(ShapeComponent)
	{
		BEGIN_REFLECT(ShapeComponent);
	}

} // namespace BHive