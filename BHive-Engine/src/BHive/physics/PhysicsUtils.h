#pragma once

#include "LockAxis.h"
#include "core/Core.h"
#include "core/math/Transform.h"

#pragma warning(push, 0)
#include <physx/PxRigidDynamic.h>
#include <physx/foundation/PxTransform.h>
#pragma warning(pop)

namespace BHive
{
	struct PhysicsUtils
	{
		static physx::PxTransform Convert(const FTransform &transform);

		static physx::PxVec3 Convert(const glm::vec3 &v);

		static FTransform Convert(const physx::PxTransform &transform);

		static physx::PxRigidDynamicLockFlags GetLockFlags(ELockAxis linear, ELockAxis angular);
	};

} // namespace BHive