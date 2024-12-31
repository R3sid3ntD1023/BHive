#pragma once

#include "SceneComponent.h"
#include "scene/components/IRenderable.h"
#include "math/AABB.hpp"

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

}