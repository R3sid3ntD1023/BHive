#include "Skeleton.h"

namespace BHive
{
	Skeleton::Skeleton(const BoneInfo &boneInfo, const SkeletalNode &root)
		: mBoneInfo(boneInfo),
		  mRoot(root)
	{
		CalculateRestPoseTransforms(mRoot, {});
	}

	const Bone *Skeleton::FindBone(uint64_t hash) const
	{
		if (mBoneInfo.Bones.contains(hash))
			return &mBoneInfo.Bones.at(hash);

		return nullptr;
	}

	void Skeleton::CalculateRestPoseTransforms(const SkeletalNode &node, const AnimTransform &parent)
	{
		auto name = node.NameHash;
		auto children = node.Children;
		auto transform = node.Transformation;

		if (mBoneInfo.Bones.contains(name))
		{
			auto &bone = mBoneInfo.Bones.at(name);
			mRestPoseTransforms.push_back(parent * transform * bone.Offset);
		}

		const auto global = parent * transform;
		for (auto &child : children)
		{
			CalculateRestPoseTransforms(child, global);
		}
	}

	REFLECT(Skeleton)
	{
		BEGIN_REFLECT(Skeleton)
		REFLECT_CONSTRUCTOR()
		REFLECT_PROPERTY_READ_ONLY("BoneInfo", mBoneInfo)
		REFLECT_PROPERTY_READ_ONLY("Heirarchy", mRoot);
	}

	REFLECT(SkeletalNode)
	{
		BEGIN_REFLECT(SkeletalNode)
		REFLECT_PROPERTY_READ_ONLY("Name", NameHash)
		REFLECT_PROPERTY_READ_ONLY("Transformation", Transformation)
		REFLECT_PROPERTY_READ_ONLY("Children", Children);
	}
} // namespace BHive
