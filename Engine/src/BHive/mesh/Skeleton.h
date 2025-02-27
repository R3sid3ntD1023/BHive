#pragma once

#include "core/Core.h"
#include "Bone.h"
#include "SkeletalNode.h"
#include "asset/Asset.h"

namespace BHive
{
	typedef std::map<std::string, Bone> Bones;
	typedef std::vector<SkeletalNode> SkeletalNodes;

	class Skeleton : public Asset
	{
	public:
		Skeleton() = default;
		Skeleton(const Bones &bones, const SkeletalNode &root);

		const SkeletalNode &GetRoot() const { return mRoot; }

		size_t GetBoneCount() const { return mBones.size(); }

		const Bone *FindBone(const std::string &name) const;

		const std::vector<glm::mat4> &GetRestPoseTransforms() const { return mRestPoseTransforms; };

		Bones &GetBones() { return mBones; }
		const Bones &GetBones() const { return mBones; }

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;

		virtual void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLEV()

	private:
		void CalculateRestPoseTransforms(const SkeletalNode &node);

	private:
		SkeletalNode mRoot;
		Bones mBones;
		std::vector<glm::mat4> mRestPoseTransforms;
	};

	REFLECT_EXTERN(Skeleton)
	REFLECT_EXTERN(Bone);
	REFLECT_EXTERN(SkeletalNode);

} // namespace BHive