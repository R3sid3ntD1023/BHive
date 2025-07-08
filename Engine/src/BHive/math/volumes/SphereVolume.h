#include "math/Math.h"
#include "Volume.h"

namespace BHive
{
	struct FPlane;
	struct AABB;

	struct FSphereVolume : public FVolume
	{
		glm::vec3 Center = {0.f, 0.f, 0.f};

		float Radius = {0.f};

		FSphereVolume(const glm::vec3 &center, float radius);

		virtual bool InFrustum(const Frustum &frustum, const FTransform &transform) const override;

	private:
		bool IsOnOrForwardPlane(const FPlane &plane) const;
	};

	FSphereVolume GenerateSphereFromAABB(const AABB &aabb);
} // namespace BHive