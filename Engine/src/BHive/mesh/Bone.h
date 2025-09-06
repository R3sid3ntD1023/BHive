#pragma once

#include "core/Core.h"

#define INVALID_BONE_INDEX 255

namespace BHive
{
	struct BHIVE_API Bone
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

	REFLECT_INLINE(Bone)
	{
		BEGIN_REFLECT(Bone)
		REFLECT_PROPERTY_READ_ONLY("Name", Name)
		REFLECT_PROPERTY_READ_ONLY("ID", ID)
		REFLECT_PROPERTY_READ_ONLY("LocalMatrix", LocalBindPoseMatrix);
	}

} // namespace BHive