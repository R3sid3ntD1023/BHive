#include "SkeletalMesh.h"

namespace BHive
{

	SkeletalMesh::SkeletalMesh(const FMeshData &mesh_data, const Ref<Skeleton> &skeleton)
		: BaseMesh(mesh_data),
		  mSkeleton(skeleton)
	{
		mDefaultPose = CreateRef<SkeletalPose>(skeleton.get());
	}

	void SkeletalMesh::Save(cereal::BinaryOutputArchive &ar) const
	{
		BaseMesh::Save(ar);

		TAssetHandle<Skeleton> handle = mSkeleton;

		ar(handle);
	}

	void SkeletalMesh::Load(cereal::BinaryInputArchive &ar)
	{
		BaseMesh::Load(ar);

		TAssetHandle<Skeleton> handle(mSkeleton);
		ar(handle);

		if (mSkeleton)
			mDefaultPose = CreateRef<SkeletalPose>(mSkeleton.get());
	}

	REFLECT(SkeletalMesh)
	{
		BEGIN_REFLECT(SkeletalMesh)
		REFLECT_CONSTRUCTOR();
	}
} // namespace BHive