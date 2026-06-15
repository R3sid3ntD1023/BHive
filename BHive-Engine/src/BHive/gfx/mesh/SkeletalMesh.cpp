#include "SkeletalMesh.h"

namespace BHive
{

	SkeletalMesh::SkeletalMesh(const FMeshData &mesh_data, const Ref<Skeleton> &skeleton)
		: BaseMesh(mesh_data),
		  mSkeleton(skeleton)
	{
		mDefaultPose = CreateRef<SkeletalPose>(mSkeleton.get());
	}

	AABB SkeletalMesh::GetBoundingBox() const
	{
		const auto &root_transform = mSkeleton->GetRoot().mTransformation;
		// const auto &root_transform = mSkeleton->FindBone(root_node)->LocalBindPoseMatrix;
		const auto &bounds = GetData().mBoundingBox;

		auto min = root_transform * glm::vec4(bounds.Min, 1);
		auto max = root_transform * glm::vec4(bounds.Max, 1);

		return {min, max};
	}

	void SkeletalMesh::Save(cereal::BinaryOutputArchive &ar) const
	{
		BaseMesh::Save(ar);

		ar(TAssetHandle(mSkeleton));
	}

	void SkeletalMesh::Load(cereal::BinaryInputArchive &ar)
	{
		BaseMesh::Load(ar);

		ar(TAssetHandle(mSkeleton));

		if (mSkeleton)
			mDefaultPose = CreateRef<SkeletalPose>(mSkeleton.get());
	}

	REFLECT(SkeletalMesh)
	{
		BEGIN_REFLECT(SkeletalMesh)
		REFLECT_CONSTRUCTOR();
		rttr::type::register_wrapper_converter_for_base_classes<Ref<SkeletalMesh>>();
	}
} // namespace BHive