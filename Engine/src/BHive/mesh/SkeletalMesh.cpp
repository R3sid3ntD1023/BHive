#include "SkeletalMesh.h"
#include "Skeleton.h"
#include "SkeletalPose.h"
#include "gfx/Shader.h"

namespace BHive
{

	SkeletalMesh::SkeletalMesh(const FMeshData &mesh_data, const Ref<Skeleton> &skeleton)
		: StaticMesh(mesh_data), mSkeleton(skeleton)
	{
		mDefaultPose = CreateRef<SkeletalPose>(skeleton.get());
	}

	void SkeletalMesh::Serialize(StreamWriter &ar) const
	{
		StaticMesh::Serialize(ar);
		TAssetHandle<Skeleton> handle = mSkeleton;

		ar(handle);
	}

	void SkeletalMesh::Deserialize(StreamReader &ar)
	{
		StaticMesh::Deserialize(ar);
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
}