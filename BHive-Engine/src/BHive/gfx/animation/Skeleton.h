#pragma once

#include "Bone.h"
#include "SkeletalNode.h"
#include "asset/Asset.h"
#include "core/Core.h"

namespace BHive
{
	typedef std::map<std::string, Bone> Bones;
	typedef std::vector<SkeletalNode> SkeletalNodes;

	struct BoneInfo
	{
		std::unordered_map<uint64_t, Bone> Bones;
		std::unordered_map<std::string, uint64_t> BoneNames;

		template <typename Ar>
		void Serialize(Ar &ar)
		{
			ar(Bones, BoneNames);
		}
	};

	class BHIVE_API Skeleton : public Asset
	{
	public:
		Skeleton() = default;
		Skeleton(const BoneInfo &boneInfo, const SkeletalNode &root);

		const SkeletalNode &GetRoot() const { return mRoot; }

		size_t GetBoneCount() const { return mBoneInfo.Bones.size(); }

		const Bone *FindBone(uint64_t hash) const;

		const std::vector<glm::mat4> &GetRestPoseTransforms() const { return mRestPoseTransforms; };

		auto &GetBones() { return mBoneInfo.Bones; }

		const auto &GetBones() const { return mBoneInfo.Bones; }

		REFLECTABLEV(Asset)

	private:
		void CalculateRestPoseTransforms(const SkeletalNode &node, const glm::mat4 &parent);

	private:
		SkeletalNode mRoot;
		BoneInfo mBoneInfo;
		std::vector<glm::mat4> mRestPoseTransforms;
	};

	REFLECT_EXTERN(Skeleton)
	REFLECT_EXTERN(Bone);
	REFLECT_EXTERN(SkeletalNode);

} // namespace BHive