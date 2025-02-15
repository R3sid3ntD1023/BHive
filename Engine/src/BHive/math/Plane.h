#pragma once

#include "Math.h"

namespace BHive
{
	struct FPlane
	{
		FPlane()
			: Normal(0, 1, 0)
		{
		}
		FPlane(const glm::vec3 &A, const glm::vec3 &B, const glm::vec3 &C, const glm::vec3 &D)
		{
			auto x = B - A;
			auto y = C - A;
			auto z = glm::cross(x, y);
			Normal = z / glm::length(z);
			Origin = (A + B + C + D) / 4.f;
		}
		FPlane(const glm::vec3 &normal)
			: Normal(normal)
		{
		}

		glm::vec3 Normal;
		glm::vec3 Origin;
	};
} // namespace BHive
