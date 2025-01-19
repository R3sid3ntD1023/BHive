#include "Animator/anim_graph/AnimGraph.h"
#include "Animator/anim_player/AnimPlayer.h"
#include "mesh/SkeletalPose.h"
#include "scene/SceneRenderer.h"
#include "SkeletalMeshComponent.h"

namespace BHive
{
	SkeletalMeshComponent::SkeletalMeshComponent(const TAssetHandle<SkeletalMesh> &mesh)
	{
		SetSkeletalMesh(mesh);
	}

	void SkeletalMeshComponent::SetSkeletalMesh(const TAssetHandle<SkeletalMesh> &mesh)
	{
		if (mesh)
		{
			SetMaterialTable(mesh->GetMaterialTable());
		}

		mSkeletalMesh = mesh;

		if (mSkeletalMesh)
		{
			mPose = CreateRef<SkeletalPose>(mSkeletalMesh->GetSkeleton().get());

			if (mAnimGraph)
			{
				mAnimatorInstance = CreateRef<AnimPlayer>(*mAnimGraph);
			}
		}
	}

	const TAssetHandle<SkeletalMesh> &SkeletalMeshComponent::GetSkeletalMesh() const
	{
		return mSkeletalMesh;
	}

	AABB SkeletalMeshComponent::GetBoundingBox() const
	{
		return mSkeletalMesh ? mSkeletalMesh->GetBoundingBox() : AABB();
	}

	void SkeletalMeshComponent::OnRender(SceneRenderer *renderer)
	{
		if (mSkeletalMesh)
			renderer->SubmitSkeletalMesh(mSkeletalMesh, GetWorldTransform(), GetBoneTransforms(), mOverrideMaterials);
	}

	const std::vector<glm::mat4> &SkeletalMeshComponent::GetBoneTransforms() const
	{
		if (mPose)
		{
			return mPose->GetTransformsJointSpace();
		}

		return mSkeletalMesh->GetDefaultPose()->GetTransformsJointSpace();
	}

	void SkeletalMeshComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		MeshComponent::Save(ar);
		ar(mSkeletalMesh);
	}

	void SkeletalMeshComponent::Load(cereal::BinaryInputArchive &ar)
	{
		MeshComponent::Load(ar);
		ar(mSkeletalMesh);
		SetSkeletalMesh(mSkeletalMesh);
	}

	REFLECT(SkeletalMeshComponent)
	{
		BEGIN_REFLECT(SkeletalMeshComponent)
		(META_DATA(ClassMetaData_ComponentSpawnable, true)) REQUIRED_COMPONENT_FUNCS()
			REFLECT_PROPERTY("SkeletalMesh", GetSkeletalMesh, SetSkeletalMesh) REFLECT_PROPERTY("AnimGraph", mAnimGraph);
	}
} // namespace BHive