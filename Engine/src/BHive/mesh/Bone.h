#pragma once

#include "core/Core.h"

#define INVALID_BONE_INDEX 255

namespace BHive
{
	struct Bone
	{
		std::string Name;
		int32_t ID;

		// glm::mat4 GlobalBindPoseMatrix;
		glm::mat4 LocalBindPoseMatrix;

		// uint8_t Parent = INVALID_BONE_INDEX;
		// std::vector<uint8_t> Children;

		// void update_bind_pose(const glm::mat4 &parent_bind_pose_matrix);

		template <typename A>
		inline void Serialize(A &ar)
		{
			ar(Name, ID, LocalBindPoseMatrix);
		}
	};

} // namespace BHive