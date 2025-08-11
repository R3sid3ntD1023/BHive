#include "PhysicsUtils.h"
#include "core/math/Transform.h"

namespace BHive
{
	physx::PxTransform PhysicsUtils::Convert(const FTransform &transform)
	{
		const auto &t = transform.GetTranslation();
		const auto &o = transform.GetQuaternion();

		physx::PxVec3 pos(t.x, t.y, t.z);
		physx::PxQuat orientation(o.x, o.y, o.z, o.w);
		return physx::PxTransform(pos, orientation);
	}

	FTransform PhysicsUtils::Convert(const physx::PxTransform &transform)
	{
		const auto &t = transform.p;
		const auto &o = transform.q;

		glm::vec3 pos(t.x, t.y, t.z);
		glm::quat orientation(o.w, o.x, o.y, o.z);
		return {pos, orientation};
	}

	physx::PxRigidDynamicLockFlags PhysicsUtils::GetLockFlags(ELockAxis linear, ELockAxis angular)
	{
		uint8_t flags = 0;

		flags |= ((linear & ELockAxis::AxisX) != 0) ? BIT(0) : 0;
		flags |= ((linear & ELockAxis::AxisY) != 0) ? BIT(1) : 0;
		flags |= ((linear & ELockAxis::AxisZ) != 0) ? BIT(2) : 0;
		flags |= ((angular & ELockAxis::AxisX) != 0) ? BIT(3) : 0;
		flags |= ((angular & ELockAxis::AxisY) != 0) ? BIT(4) : 0;
		flags |= ((angular & ELockAxis::AxisX) != 0) ? BIT(5) : 0;

		return (physx::PxRigidDynamicLockFlags)flags;
	}
} // namespace BHive