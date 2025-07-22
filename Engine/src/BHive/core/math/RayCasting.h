#pragma once

#include "Math.h"
#include "boundingbox/AABB.h"

namespace BHive
{
	struct FRay
	{
		glm::vec3 Origin;
		glm::vec3 Direction;
	};

	struct RayCast
	{
		static glm::vec3 GetPointOnRay(const glm::vec3 &origin, const glm::vec3 &direction, float distance);

		static bool IsRayIntersectingSphere(const FRay &ray, const FSphere &aabb);

		static bool IsRayIntersectingBox(const FRay &ray, const FBox &aabb);

		static float GetRayPlaneDistance(const FRay &ray, const glm::vec3 &normal, float distance);
	};
} // namespace BHive