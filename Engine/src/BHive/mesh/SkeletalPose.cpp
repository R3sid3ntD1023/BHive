#include "SkeletalPose.h"
#include "Skeleton.h"
#include "core/math/MathFunctionLibrary.h"

namespace BHive
{
	SkeletalPose::SkeletalPose(const Skeleton *skeleton, type pose_type)
		: mSkeleton(skeleton)
	{
		Reset(pose_type);
	}

	void SkeletalPose::Reset(type pose_type)
	{
		auto count{mSkeleton->GetBoneCount()};

		if (pose_type == type::absolute)
		{
			mTransformsJointSpace = mSkeleton->GetRestPoseTransforms();
		}
		else
		{
			mTransformsJointSpace.resize(count);
			std::fill(mTransformsJointSpace.begin(), mTransformsJointSpace.end(), glm::identity<glm::mat4>());
		}
	}

	void SkeletalPose::SetTransformJointSpace(uint64_t index, const glm::mat4 &transform)
	{
		mTransformsJointSpace[index] = transform;
	}

	inline const glm::mat4 &SkeletalPose::GetTransformJointSpace(uint64_t index) const
	{
		return mTransformsJointSpace[index];
	}

	void SkeletalPose::RecalulateObjectSpaceTransforms()
	{
		ReadHeirarchy(mSkeleton->GetRoot(), glm::mat4(1.0f));
	}

	void SkeletalPose::ReadHeirarchy(const SkeletalNode &node, const glm::mat4 &parent)
	{
		auto name = node.mName;
		auto transform = node.mTransformation;
		auto bone = mSkeleton->FindBone(name);

		if (auto bone = mSkeleton->FindBone(name))
		{
			auto index = bone->mID;
			transform = GetTransformJointSpace(index);
			SetTransformJointSpace(index, parent * transform * bone->mOffset);
		}

		glm::mat4 global_transform = parent * transform;
		for (auto &child : node.mChildren)
		{
			ReadHeirarchy(child, global_transform);
		}
	}

	void SkeletalPoseBlend(const SkeletalPose &p0, const SkeletalPose &p1, float weight, SkeletalPose &result)
	{
		ASSERT(p0.GetBoneCount() == p1.GetBoneCount());

		const auto &bone_count = p0.GetBoneCount();

		for (uint64_t i = 0; i < bone_count; i++)
		{
			const FTransform &t0 = p0.GetTransformJointSpace(i);
			const FTransform &t1 = p1.GetTransformJointSpace(i);

			result.SetTransformJointSpace(i, MathFunctionLibrary::mix(t0, t1, weight));
		}
	}

	void SkeletalPoseAdd(const SkeletalPose &p0, const SkeletalPose &p1, SkeletalPose &result)
	{
		ASSERT(p0.GetBoneCount() == p1.GetBoneCount());

		const auto &bone_count = p0.GetBoneCount();

		for (uint64_t i = 0; i < bone_count; i++)
		{
			const glm::mat4 &t0 = p0.GetTransformJointSpace(i);
			const glm::mat4 &t1 = p1.GetTransformJointSpace(i);

			result.SetTransformJointSpace(i, t0 * t1);
		}
	}
} // namespace BHive