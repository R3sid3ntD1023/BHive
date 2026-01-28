#pragma once

#include "core/Core.h"
#include "LockAxis.h"
#include "core/math/Transform.h"
#include <physx/foundation/PxTransform.h>
#include <physx/PxRigidDynamic.h>

namespace BHive
{
	struct PhysicsUtils
	{
		static physx::PxTransform Convert(const FTransform &transform);

		static FTransform Convert(const physx::PxTransform &transform);

		static physx::PxRigidDynamicLockFlags GetLockFlags(ELockAxis linear, ELockAxis angular);
	};

} // namespace BHive