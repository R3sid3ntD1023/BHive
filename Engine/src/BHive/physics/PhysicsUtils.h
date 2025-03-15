#pragma once

#include "math/Transform.h"
#include "PhysicsCore.h"
#include "LockAxis.h"

namespace BHive
{
	namespace physics::utils
	{
		inline rp3d::Transform GetPhysicsTransform(const FTransform &t)
		{
			auto position = t.get_translation();
			auto rotation = glm::radians(t.get_rotation());

			auto quaternion = rp3d::Quaternion::fromEulerAngles(rp3d::Vector3(rotation.x, rotation.y, rotation.z));
			return rp3d::Transform({position.x, position.y, position.z}, quaternion);
		}

		inline glm::quat rp3d_to_quat(const rp3d::Quaternion &qua)
		{
			return glm::quat((float)qua.w, (float)qua.x, (float)qua.y, (float)qua.z);
		}

		inline rp3d::Vector3 vec3_to_rp3d(const glm::vec3 &v)
		{
			return rp3d::Vector3(v.x, v.y, v.z);
		}

		inline FTransform GetTransform(const rp3d::Transform &t, const glm::vec3 &scale)
		{
			auto &position = t.getPosition();
			auto quat = rp3d_to_quat(t.getOrientation());
			glm::vec3 rotation = glm::degrees(glm::eulerAngles(quat));

			return FTransform({position.x, position.y, position.z}, {rotation.x, rotation.y, rotation.z}, scale);
		}

		inline rp3d::Vector3 LockAxisToVextor3(ELockAxis axis)
		{
			float x = (axis & ELockAxis::AxisX) != 0 ? 0.0f : 1.0f;
			float y = (axis & ELockAxis::AxisY) != 0 ? 0.0f : 1.0f;
			float z = (axis & ELockAxis::AxisZ) != 0 ? 0.0f : 1.0f;

			return {x, y, z};
		}
	} // namespace physics::utils

} // namespace BHive