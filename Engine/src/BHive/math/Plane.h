#pragma once

#include "Math.h"

namespace BHive
{
	struct FPlane
	{
		FPlane() = default;
		FPlane(const glm::vec3 &p, const glm::vec3 &normal)
			: Normal(glm::normalize(normal)),
			  Distance(glm::dot(normal, p))
		{
		}

		FPlane(const glm::vec3 &A, const glm::vec3 &B, const glm::vec3 &C, const glm::vec3 &D)
		{
			auto ab = A - B;
			auto ac = A - C;
			auto per = glm::cross(ab, ac);
			Normal = glm::normalize(per);
			Origin = (A + B + C + D) / 4.f;

			Distance = glm::dot(Normal, D);
		}

		float GetSignedDistanceToPlane(const glm::vec3 &point) const { return glm::dot(Normal, point) - Distance; }

		glm::vec3 Normal = {0, 1, 0};
		glm::vec3 Origin = {0, 0, 0};
		float Distance = 0.f;
	};
} // namespace BHive
