#include "SkeletalMesh.h"

namespace BHive
{

	SkeletalMesh::SkeletalMesh(const FMeshData &mesh_data, Skeleton *skeleton)
		: BaseMesh(mesh_data),
		  mSkeleton(skeleton)
	{
		mDefaultPose = CreateRef<SkeletalPose>(mSkeleton);
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

	REFLECT(SkeletalMesh)
	{
		BEGIN_REFLECT(SkeletalMesh)
		REFLECT_CONSTRUCTOR();
		rttr::type::register_wrapper_converter_for_base_classes<Ref<SkeletalMesh>>();
	}
} // namespace BHive