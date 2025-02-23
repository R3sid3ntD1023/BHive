#pragma once

#include "Math.h"

namespace BHive
{
	struct FPlane
	{
		glm::vec3 Normal{0, 1, 0};
		float Distance{0};
	};

	inline FPlane CreatePlane(const glm::vec3 &p1, const glm::vec3 &normal)
	{
		FPlane p;
		p.Normal = glm::normalize(normal);
		p.Distance = glm::dot(p.Normal, p1);

		return p;
	}

	inline float GetSignedDistanceToPlane(const FPlane &plane, const glm::vec3 &point)
	{
		return glm::dot(plane.Normal, point) - plane.Distance;
	}
} // namespace BHive
