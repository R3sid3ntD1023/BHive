#include "Skeleton.h"

namespace BHive
{
	Skeleton::Skeleton(const Bones &bones, const SkeletalNode &root)
		: mBones(bones),
		  mRoot(root)
	{
		CalculateRestPoseTransforms(mRoot);
	}

	const Bone *Skeleton::FindBone(const std::string &name) const
	{
		if (mBones.contains(name))
			return &mBones.at(name);

		return nullptr;
	}

	void Skeleton::Save(cereal::BinaryOutputArchive &ar) const
	{
		Asset::Save(ar);
		ar(mBones, mRoot, mRestPoseTransforms);
	}

	void Skeleton::Load(cereal::BinaryInputArchive &ar)
	{
		Asset::Load(ar);
		ar(mBones, mRoot, mRestPoseTransforms);
	}

	void Skeleton::CalculateRestPoseTransforms(const SkeletalNode &node)
	{
		auto name = node.mName;
		auto children = node.mChildren;
		auto transform = node.mTransformation;
		auto parent_transform = node.ParentTransformation;

		if (mBones.contains(name))
		{
			auto &bone = mBones.at(name);
			mRestPoseTransforms.push_back(transform * bone.mOffset);
		}

		for (auto &child : children)
			CalculateRestPoseTransforms(child);
	}

	REFLECT(Skeleton)
	{
		BEGIN_REFLECT(Skeleton)
		REFLECT_CONSTRUCTOR()
		REFLECT_PROPERTY_READ_ONLY("Bones", mBones)
		REFLECT_PROPERTY_READ_ONLY("Heirarchy", mRoot);
	}
	REFLECT(Bone)
	{
		BEGIN_REFLECT(Bone)
		REFLECT_PROPERTY_READ_ONLY("Name", mName)
		REFLECT_PROPERTY_READ_ONLY("ID", mID)
		REFLECT_PROPERTY_READ_ONLY("Offset", mOffset)
		REFLECT_PROPERTY_READ_ONLY("Parent", mParent);
	}

	REFLECT(SkeletalNode)
	{
		BEGIN_REFLECT(SkeletalNode)
		REFLECT_PROPERTY_READ_ONLY("Name", mName)
		REFLECT_PROPERTY_READ_ONLY("Transformation", mTransformation)
		REFLECT_PROPERTY_READ_ONLY("Children", mChildren);
	}
} // namespace BHive
