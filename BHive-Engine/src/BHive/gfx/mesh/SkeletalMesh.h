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
		SkeletalMesh(const FMeshData &data, const Ref<Skeleton> &skeleton);

		Ref<Skeleton> GetSkeleton() { return mSkeleton; }

		Ref<class SkeletalPose> GetDefaultPose() const { return mDefaultPose; }

		const AABB &GetBoundingBox() const override;

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;

		virtual void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLEV(BaseMesh)

	private:
		Ref<Skeleton> mSkeleton;
		Ref<class SkeletalPose> mDefaultPose;
	};

	REFLECT_EXTERN(SkeletalMesh)
} // namespace BHive
