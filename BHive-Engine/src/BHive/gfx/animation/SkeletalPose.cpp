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
			std::fill(mTransformsJointSpace.begin(), mTransformsJointSpace.end(), AnimTransform{});
		}
	}

	void SkeletalPose::SetTransformJointSpace(uint64_t index, const AnimTransform &transform)
	{
		mTransformsJointSpace[index] = transform;
	}

	const auto &SkeletalPose::GetTransformJointSpace(uint64_t index) const
	{
		return mTransformsJointSpace[index];
	}

	void SkeletalPose::RecalulateObjectSpaceTransforms()
	{
		ReadHeirarchy(mSkeleton->GetRoot(), {});
	}

	void SkeletalPose::ReadHeirarchy(const SkeletalNode &node, const AnimTransform &parent)
	{
		auto name = node.NameHash;
		auto transform = node.Transformation;

		if (auto bone = mSkeleton->FindBone(name))
		{
			auto index = bone->ID;
			transform = GetTransformJointSpace(index);
			SetTransformJointSpace(index, parent * transform * bone->Offset);
		}

		const auto global_transform = parent * transform;
		for (auto &child : node.Children)
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
			const auto &t0 = p0.GetTransformJointSpace(i);
			const auto &t1 = p1.GetTransformJointSpace(i);

			result.SetTransformJointSpace(i, MathFunctionLibrary::Lerp(t0, t1, weight));
		}
	}

	void SkeletalPoseAdd(const SkeletalPose &p0, const SkeletalPose &p1, SkeletalPose &result)
	{
		ASSERT(p0.GetBoneCount() == p1.GetBoneCount());

		const auto &bone_count = p0.GetBoneCount();

		for (uint64_t i = 0; i < bone_count; i++)
		{
			const auto &t0 = p0.GetTransformJointSpace(i);
			const auto &t1 = p1.GetTransformJointSpace(i);

			result.SetTransformJointSpace(i, t0 * t1);
		}
	}
} // namespace BHive