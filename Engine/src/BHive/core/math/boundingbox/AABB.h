#pragma once

#include "core/Core.h"
#include "core/math/Math.h"

namespace BHive
{
	struct BHIVE_API FBox
	{
		glm::vec3 Min{0.0f};
		glm::vec3 Max{0.0f};
	};

	struct BHIVE_API FSphere
	{
		glm::vec3 Origin{0.0f};
		float Radius{0.0f};
	};

	struct BHIVE_API AABB
	{

		glm::vec3 Min{0.0f};
		glm::vec3 Max{0.0f};

		AABB() = default;

		AABB(const FSphere &sphere);

		AABB(const FBox &box);

		AABB(const glm::vec3 &min, const glm::vec3 &max);

		glm::vec3 get_center() const;

		glm::vec3 get_extent() const;

		float get_radius() const;

		FBox get_box() const;

		FSphere get_sphere() const;

		AABB &operator=(const AABB &rhs);

		AABB operator+(const AABB &rhs) const;

		AABB operator+(const glm::vec3 &rhs) const;

		AABB operator*(const glm::vec3 &rhs) const;

		AABB &operator+=(const AABB &rhs);

		template <typename A>
		void Serialize(A &ar)
		{
			ar(Min, Max);
		}
	};
} // namespace BHive