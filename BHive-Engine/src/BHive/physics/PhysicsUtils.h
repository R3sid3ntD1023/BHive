#pragma once

#include "core/Core.h"
#include "LockAxis.h"
#include "core/math/Transform.h"

#pragma warning(push, 0)
#include <physx/foundation/PxTransform.h>
#include <physx/PxRigidDynamic.h>
#pragma warning(pop)

namespace BHive
{
	struct PhysicsUtils
	{
		static physx::PxTransform Convert(const FTransform &transform);

		static FTransform Convert(const physx::PxTransform &transform);

		static physx::PxRigidDynamicLockFlags GetLockFlags(ELockAxis linear, ELockAxis angular);
	};

} // namespace BHive