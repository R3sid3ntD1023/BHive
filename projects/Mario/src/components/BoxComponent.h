#pragma once

#include "math/Transform.h"
#include "math/AABB.hpp"
#include "ColliderComponent.h"

namespace BHive
{
	struct BoxComponent : public ColliderComponent
	{
		glm::vec3 Extents{0.5f};

		virtual void Render() override;

		virtual AABB GetBoundingBox() const { return FBox{{}, Extents}; }

		void *GetCollisionShape(const FTransform &world_transform) override;
		void OnReleaseCollisionShape() override;

		REFLECTABLEV(ColliderComponent)
	};

} // namespace BHive