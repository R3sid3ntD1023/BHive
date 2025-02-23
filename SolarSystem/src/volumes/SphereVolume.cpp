#include "math/AABB.hpp"
#include "math/Frustum.h"
#include "math/Transform.h"
#include "SphereVolume.h"

namespace BHive
{
	FSphereVolume::FSphereVolume(const glm::vec3 &center, float radius)
		: Center(center),
		  Radius(radius)
	{
	}

	bool FSphereVolume::InFrustum(const Frustum &frustum, const FTransform &transform) const
	{
		const glm::vec3 globalScale = transform.get_scale();

		const glm::vec3 globalCenter = transform.to_mat4() * glm::vec4(Center, 1.0f);

		const float maxScale = glm::compMax(globalScale);

		FSphereVolume sphere(globalCenter, Radius * (maxScale * 0.5f));

		const auto &planes = frustum._get_planes();
		return (
			sphere.IsOnOrForwardPlane(planes[0]) && sphere.IsOnOrForwardPlane(planes[1]) &&
			sphere.IsOnOrForwardPlane(planes[2]) && sphere.IsOnOrForwardPlane(planes[3]) &&
			sphere.IsOnOrForwardPlane(planes[4]) && sphere.IsOnOrForwardPlane(planes[5]));
	}

	bool FSphereVolume::IsOnOrForwardPlane(const FPlane &plane) const
	{
		return plane.GetSignedDistanceToPlane(Center) > -Radius;
	}

	FSphereVolume GenerateSphereFromAABB(const AABB &aabb)
	{
		return FSphereVolume(aabb.get_center(), aabb.get_radius());
	}

} // namespace BHive