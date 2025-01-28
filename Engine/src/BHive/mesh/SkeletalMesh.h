#pragma once

#include "IRenderableAsset.h"
#include "SkeletalPose.h"
#include "Skeleton.h"

namespace BHive
{
	class Skeleton;

	class SkeletalMesh : public IRenderableAsset
	{
	public:
		SkeletalMesh() = default;
		SkeletalMesh(const FMeshData &data, const Ref<Skeleton> &skeleton);

		Ref<Skeleton> GetSkeleton() { return mSkeleton; }

		Ref<class SkeletalPose> GetDefaultPose() { return mDefaultPose; }

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;

		virtual void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLEV(IRenderableAsset)

	private:
		Ref<Skeleton> mSkeleton;
		Ref<class SkeletalPose> mDefaultPose;
	};

	REFLECT_EXTERN(SkeletalMesh)
} // namespace BHive
