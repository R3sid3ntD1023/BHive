#include "AABB.h"

namespace BHive
{
	AABB::AABB(const FSphere &sphere)
		: Min(sphere.Origin - sphere.Radius),
		  Max(sphere.Origin + sphere.Radius)
	{
	}

	AABB::AABB(const FBox &box)
		: Min(box.Min),
		  Max(box.Max)
	{
	}

	AABB::AABB(const glm::vec3 &min, const glm::vec3 &max)
		: Min(min),
		  Max(max)
	{
	}

	glm::vec3 AABB::get_center() const
	{
		return (Max + Min) * 0.5f;
	}

	glm::vec3 AABB::get_extent() const
	{
		return abs(Max - Min) * 0.5f;
	}

	float AABB::get_radius() const
	{
		return glm::length(get_extent());
	}

	FBox AABB::get_box() const
	{
		return {Min, Max};
	}

	FSphere AABB::get_sphere() const
	{
		return {Min + get_radius(), get_radius()};
	}

	AABB &AABB::operator=(const AABB &rhs)
	{
		Min = rhs.Min;
		Max = rhs.Max;
		return *this;
	}

	AABB AABB::operator+(const AABB &rhs) const
	{
		return {glm::min(Min, rhs.Min), glm::max(Max, rhs.Max)};
	}

	AABB AABB::operator+(const glm::vec3 &rhs) const
	{
		return {Min + rhs, Max + rhs};
	}

	AABB AABB::operator*(const glm::vec3 &rhs) const
	{
		return {Min * rhs, Max * rhs};
	}

	AABB &AABB::operator+=(const AABB &rhs)
	{
		Min = glm::min(Min, rhs.Min);
		Max = glm::max(Max, rhs.Max);

		return *this;
	}
} // namespace BHive