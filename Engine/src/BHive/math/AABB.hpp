#pragma once

#include "math/Math.h"

namespace BHive
{
	struct BHIVE FBox
	{
		glm::vec3 Min{0.0f};
		glm::vec3 Max{0.0f};
	};

	struct BHIVE FSphere
	{
		glm::vec3 Origin{0.0f};
		float Radius{0.0f};
	};

	struct BHIVE AABB
	{

		glm::vec3 Min{0.0f};
		glm::vec3 Max{0.0f};

		AABB() = default;
		AABB(const FSphere &sphere)
			: Min(sphere.Origin - sphere.Radius),
			  Max(sphere.Origin + sphere.Radius)
		{
		}

		AABB(const FBox &box)
			: Min(box.Min),
			  Max(box.Max)
		{
		}

		AABB(const glm::vec3 &min, const glm::vec3 &max)
			: Min(min),
			  Max(max)
		{
		}

		glm::vec3 get_center() const { return (Max + Min) * 0.5f; }

		glm::vec3 get_extent() const { return abs(Max - Min) * 0.5f; }

		float get_radius() const { return glm::length(get_extent()); }

		FBox GetBox() const { return {Min, Max}; }

		FSphere GetSphere() const { return {Min + get_radius(), get_radius()}; }

		AABB &operator=(const AABB &rhs)
		{
			Min = rhs.Min;
			Max = rhs.Max;
			return *this;
		}

		AABB operator+(const AABB &rhs) const { return {glm::min(Min, rhs.Min), glm::max(Max, rhs.Max)}; }

		AABB operator+(const glm::vec3 &rhs) const { return {Min + rhs, Max + rhs}; }

		AABB operator*(const glm::vec3 &rhs) const { return {Min * rhs, Max * rhs}; }

		AABB &operator+=(const AABB &rhs)
		{
			Min = glm::min(Min, rhs.Min);
			Max = glm::max(Max, rhs.Max);

			return *this;
		}

		template <typename A>
		void Serialize(A &ar)
		{
			ar(Min, Max);
		}
	};
} // namespace BHive