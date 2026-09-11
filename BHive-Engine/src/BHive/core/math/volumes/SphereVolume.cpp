#include "SphereVolume.h"
#include "core/math/Frustum.h"
#include "core/math/MathFunctionLibrary.h"
#include "core/math/Transform.h"
#include "core/math/boundingbox/AABB.h"

namespace BHive
{
	FSphereVolume::FSphereVolume(const glm::vec3 &center, float radius)
		: Center(center),
		  Radius(radius)
	{
	}

	bool FSphereVolume::InFrustum(const Frustum &frustum, const FTransform &transform) const
	{
		const glm::vec3 globalCenter = transform.TransformPoint(Center);

		const float maxScale = glm::compMax(transform.Scale);

		FSphereVolume sphere(globalCenter, Radius * (maxScale * 0.5f));

		const auto planes = frustum.GetPlanes();
		for (uint32_t i = 0; i < 6; i++)
		{
			auto plane = planes[i];
			if (!sphere.IsOnOrForwardPlane(plane.xyz, plane.w))
			{
				return false;
			}
		}

		return true;
	}

	bool FSphereVolume::IsOnOrForwardPlane(const glm::vec3 &normal, float distance) const
	{
		return MathFunctionLibrary::GetSignedDistanceToPlane(normal, distance, Center) > -Radius;
	}

	FSphereVolume GenerateSphereFromAABB(const AABB &aabb)
	{
		return FSphereVolume(aabb.GetCenter(), aabb.GetRadius());
	}

} // namespace BHive