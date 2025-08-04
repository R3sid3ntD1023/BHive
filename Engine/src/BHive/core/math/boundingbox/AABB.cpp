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

	glm::vec3 AABB::GetCenter() const
	{
		return (Max + Min) * 0.5f;
	}

	glm::vec3 AABB::GetExtent() const
	{
		return abs(Max - Min) * 0.5f;
	}

	float AABB::GetRadius() const
	{
		return glm::length(GetExtent());
	}

	FBox AABB::GetBox() const
	{
		return {Min, Max};
	}

	FSphere AABB::GetSphere() const
	{
		return {Min + GetRadius(), GetRadius()};
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