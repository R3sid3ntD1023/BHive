#pragma once

#include "StaticMesh.h"

namespace BHive
{
	class Skeleton;

	class SkeletalMesh : public StaticMesh
	{
	public:
		SkeletalMesh() = default;
		SkeletalMesh(const FMeshData &data, const Ref<Skeleton> &skeleton);

		Ref<Skeleton> GetSkeleton() { return mSkeleton; }

		Ref<class SkeletalPose> GetDefaultPose() { return mDefaultPose; }

		virtual void Serialize(StreamWriter &ar) const;

		virtual void Deserialize(StreamReader &ar);

		REFLECTABLEV(StaticMesh)

	private:
		Ref<Skeleton> mSkeleton;
		Ref<class SkeletalPose> mDefaultPose;
	};

	REFLECT_EXTERN(SkeletalMesh)
} // namespace BHive
