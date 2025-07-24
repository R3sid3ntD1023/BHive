#include "Bone.h"

namespace BHive
{
	/*void Bone::update_bind_pose(const glm::mat4 &parent_bind_pose_matrix)
	{
		GlobalBindPoseMatrix = parent_bind_pose_matrix * LocalBindPoseMatrix;
	}*/

	REFLECT(Bone)
	{
		BEGIN_REFLECT(Bone)
		REFLECT_PROPERTY_READ_ONLY("Name", Name)
		REFLECT_PROPERTY_READ_ONLY("ID", ID)
		REFLECT_PROPERTY_READ_ONLY("LocalMatrix", LocalBindPoseMatrix);
	}
} // namespace BHive