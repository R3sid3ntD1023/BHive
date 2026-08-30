#pragma once

#include "BaseMesh.h"
#include "gfx/animation/SkeletalPose.h"
#include "gfx/animation/Skeleton.h"

namespace BHive
{
	class Skeleton;

	class BHIVE_API SkeletalMesh : public BaseMesh
	{
	public:
		SkeletalMesh() = default;
		SkeletalMesh(const FMeshData &data, Skeleton *skeleton);

		Skeleton *GetSkeleton() { return mSkeleton; }

		Ref<class SkeletalPose> GetDefaultPose() const { return mDefaultPose; }

		AABB GetBoundingBox() const override;

		REFLECTABLEV(BaseMesh)

	private:
		Skeleton *mSkeleton;
		Ref<class SkeletalPose> mDefaultPose;
	};

	REFLECT_EXTERN(SkeletalMesh)
} // namespace BHive
