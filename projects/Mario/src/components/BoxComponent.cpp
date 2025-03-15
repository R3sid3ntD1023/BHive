#include "BoxComponent.h"
#include "physics/PhysicsContext.h"

namespace BHive
{
	void *BoxComponent::GetCollisionShape(const FTransform &world_transform)
	{
		auto extents = mExtents * world_transform.get_scale();
		return PhysicsContext::get_context().createBoxShape(rp3d::Vector3{extents.x, extents.y, extents.z});
	}

	void BoxComponent::OnReleaseCollisionShape()
	{
		PhysicsContext::get_context().destroyBoxShape((rp3d::BoxShape *)mCollisionShape);
		mCollisionShape = nullptr;
	}

} // namespace BHive