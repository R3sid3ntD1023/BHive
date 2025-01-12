#pragma once

#include "core/Core.h"
#include "math/Transform.h"
#include "asset/Asset.h"

namespace BHive
{

	struct Transform
	{
		glm::vec3 Position;
		glm::quat Rotation;
		glm::vec3 Scale{1.0f};
	};

	struct SkeletalNode
	{
		std::string mName;
		glm::mat4 mTransformation;
		std::vector<SkeletalNode> mChildren;
	};

	struct Bone
	{
		std::string mName;
		int32_t mID;
		glm::mat4 mOffset;
		int32_t mParent = -1;
	};

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

		virtual void Serialize(StreamWriter &ar) const;

		virtual void Deserialize(StreamReader &ar);

		REFLECTABLEV()

	private:
		void CalculateRestPoseTransforms(const SkeletalNode &node, const glm::mat4 &parent);

	private:
		SkeletalNode mRoot;
		Bones mBones;
		std::vector<glm::mat4> mRestPoseTransforms;
	};

	REFLECT_EXTERN(Skeleton)
	REFLECT_EXTERN(Bone);
	REFLECT_EXTERN(SkeletalNode);

	inline void Serialize(StreamWriter &ar, const SkeletalNode &obj)
	{
		ar(obj.mName, obj.mTransformation, obj.mChildren);
	}

	inline void Deserialize(StreamReader &ar, SkeletalNode &obj)
	{
		ar(obj.mName, obj.mTransformation, obj.mChildren);
	}

	inline void Serialize(StreamWriter &ar, const Bone &obj)
	{
		ar(obj.mName, obj.mID, obj.mOffset, obj.mParent);
	}

	inline void Deserialize(StreamReader &ar, Bone &obj)
	{
		ar(obj.mName, obj.mID, obj.mOffset, obj.mParent);
	}
}