#pragma once

#include "LockAxis.h"
#include "math/Transform.h"
#include "PhysicsCore.h"
#include <physx/foundation/PxTransform.h>
#include <physx/PxRigidDynamic.h>

namespace BHive
{
	namespace physics::utils
	{

		inline physx::PxTransform getTransform(const FTransform &transform)
		{
			const auto &t = transform.get_translation();
			const auto &o = transform.get_quaternion();

			physx::PxVec3 pos(t.x, t.y, t.z);
			physx::PxQuat orientation(o.x, o.y, o.z, o.w);
			return physx::PxTransform(pos, orientation);
		}

		inline physx::PxRigidDynamicLockFlags GetLockFlags(ELockAxis linear, ELockAxis angular)
		{
			uint8_t flags = 0;

			flags |= ((linear & ELockAxis::AxisX) != 0) ? BIT(0) : 0;
			flags |= ((linear & ELockAxis::AxisY) != 0) ? BIT(1) : 0;
			flags |= ((linear & ELockAxis::AxisZ) != 0) ? BIT(2) : 0;
			flags |= ((angular & ELockAxis::AxisX) != 0) ? BIT(3) : 0;
			flags |= ((angular & ELockAxis::AxisY) != 0) ? BIT(4) : 0;
			flags |= ((angular & ELockAxis::AxisX) != 0) ? BIT(5) : 0;

			return physx::PxRigidDynamicLockFlags(flags);
		}
	} // namespace physics::utils

} // namespace BHive