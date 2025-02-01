#include "RayCasting.h"

namespace BHive
{
	glm::vec3 RayCast::GetPointOnRay(const glm::vec3 &origin, const glm::vec3 &direction, float distance)
	{
		return origin + direction * distance;
	}

	bool RayCast::IsRayIntersectingSphere(const FRay &ray, const FSphere &sphere)
	{
		auto a = glm::dot(ray.Direction, ray.Direction);
		auto b = 2 * glm::dot(ray.Origin, ray.Direction);
		auto c = glm::dot(ray.Origin, ray.Origin) - (sphere.Radius * sphere.Radius);
		auto d = (b * b) - (4.f * a * c);

		if (d >= 0.0f)
		{
			return true;
		}

		return false;
	}

	bool RayCast::IsRayIntersectingBox(const FRay &ray, const FBox &box)
	{
		float xmin = (box.Min.x - ray.Origin.x) / ray.Direction.x;
		float xmax = (box.Max.x - ray.Origin.x) / ray.Direction.x;
		float ymin = (box.Min.y - ray.Origin.y) / ray.Direction.y;
		float ymax = (box.Max.y - ray.Origin.y) / ray.Direction.y;
		float zmin = (box.Min.z - ray.Origin.z) / ray.Direction.z;
		float zmax = (box.Max.z - ray.Origin.z) / ray.Direction.z;

		float tmin = glm::max(glm::max(glm::min(xmin, xmax), glm::min(ymin, ymax)), glm::min(zmin, zmax));
		float tmax = glm::min(glm::min(glm::max(xmin, xmax), glm::max(ymin, ymax)), glm::max(zmin, zmax));

		if (tmax < 0 || tmin > tmax)
		{
			return false;
		}

		return true;
	}

	float RayCast::GetRayPlaneDistance(const FRay &ray, const glm::vec3 &normal, float distance)
	{
		float numerator = glm::dot(normal, ray.Origin) + distance;
		float denominator = glm::dot(normal, ray.Direction);

		return -(numerator / denominator);
	}
} // namespace BHive
