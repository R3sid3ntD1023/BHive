#pragma once
#include "TransformCoordinates.hpp"
#include "AABB.hpp"

namespace BHive
{
	struct BHIVE FRay
	{
		glm::vec3 Origin;
		glm::vec3 Direction;
	};

	struct RayCast
	{
		static BHIVE glm::vec3 GetPointOnRay(const glm::vec3 &origin, const glm::vec3 &direction, float distance);

		static BHIVE bool IsRayIntersectingSphere(const FRay &ray, const FSphere &aabb);

		static BHIVE bool IsRayIntersectingBox(const FRay &ray, const FBox &aabb);

		static float GetRayPlaneDistance(const FRay &ray, const glm::vec3 &normal, float distance);
	};
} // namespace BHive