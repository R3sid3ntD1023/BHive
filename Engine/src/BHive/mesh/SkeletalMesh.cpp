#include "SkeletalMesh.h"

namespace BHive
{

	SkeletalMesh::SkeletalMesh(const FMeshData &mesh_data, const Ref<Skeleton> &skeleton)
		: StaticMesh(mesh_data),
		  mSkeleton(skeleton)
	{
		mDefaultPose = CreateRef<SkeletalPose>(skeleton.get());
	}

	void SkeletalMesh::Save(cereal::JSONOutputArchive &ar) const
	{
		StaticMesh::Save(ar);

		TAssetHandle<Skeleton> handle = mSkeleton;

		ar(MAKE_NVP("Skeleton", handle));
	}

	void SkeletalMesh::Load(cereal::JSONInputArchive &ar)
	{
		StaticMesh::Load(ar);

		TAssetHandle<Skeleton> handle;
		ar(MAKE_NVP("Skeleton", handle));

		mSkeleton = handle.get();
		mDefaultPose = CreateRef<SkeletalPose>(mSkeleton.get());
	}

	REFLECT(SkeletalMesh)
	{
		BEGIN_REFLECT(SkeletalMesh)
		REFLECT_CONSTRUCTOR();
	}
} // namespace BHive