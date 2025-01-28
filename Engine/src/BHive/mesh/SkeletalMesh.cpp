#include "SkeletalMesh.h"

namespace BHive
{

	SkeletalMesh::SkeletalMesh(const FMeshData &mesh_data, const Ref<Skeleton> &skeleton)
		: IRenderableAsset(mesh_data),
		  mSkeleton(skeleton)
	{
		mDefaultPose = CreateRef<SkeletalPose>(skeleton.get());
	}

	void SkeletalMesh::Save(cereal::BinaryOutputArchive &ar) const
	{
		IRenderableAsset::Save(ar);

		TAssetHandle<Skeleton> handle = mSkeleton;

		ar(handle);
	}

	void SkeletalMesh::Load(cereal::BinaryInputArchive &ar)
	{
		IRenderableAsset::Load(ar);

		TAssetHandle<Skeleton> handle;
		ar(handle);

		mSkeleton = handle.get();
		mDefaultPose = CreateRef<SkeletalPose>(mSkeleton.get());
	}

	REFLECT(SkeletalMesh)
	{
		BEGIN_REFLECT(SkeletalMesh)
		REFLECT_CONSTRUCTOR();
	}
} // namespace BHive