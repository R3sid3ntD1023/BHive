#pragma once

#include "core/math/Transform.h"
#include "SkeletalNode.h"

namespace BHive
{

	class Skeleton;

	class SkeletalPose
	{
	public:
		enum class type
		{
			absolute,
			additive
		};

		SkeletalPose(const Skeleton *skeleton, type pose_type = type::absolute);

		const Skeleton &GetSkeleton() const { return *mSkeleton; }

		void Reset(type pose_type = type::absolute);

		void SetTransformJointSpace(uint64_t index, const glm::mat4 &transform);

		const glm::mat4 &GetTransformJointSpace(uint64_t index) const;

		const std::vector<glm::mat4> &GetTransformsJointSpace() const { return mTransformsJointSpace; }

		uint64_t GetBoneCount() const { return mTransformsJointSpace.size(); }

		void RecalulateObjectSpaceTransforms();

	private:
		void ReadHeirarchy(const SkeletalNode &node, const glm::mat4 &parent);

		const Skeleton *mSkeleton;

		std::vector<glm::mat4> mTransformsJointSpace;
	};

	void SkeletalPoseBlend(const SkeletalPose &p0, const SkeletalPose &p1, float weight, SkeletalPose &result);

	void SkeletalPoseAdd(const SkeletalPose &p0, const SkeletalPose &p1, SkeletalPose &result);
} // namespace BHive