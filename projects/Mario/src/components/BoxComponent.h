#pragma once

#include "math/Transform.h"
#include "math/AABB.hpp"
#include "ColliderComponent.h"

namespace BHive
{
	struct BoxComponent : public ColliderComponent
	{
		glm::vec3 mExtents{0.5f};

		virtual AABB GetBoundingBox() const { return FBox{{}, mExtents}; }

		void *GetCollisionShape(const FTransform &world_transform) override;
		void OnReleaseCollisionShape() override;
	};
} // namespace BHive