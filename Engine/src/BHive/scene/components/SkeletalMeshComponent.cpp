#include "SkeletalMeshComponent.h"
#include "mesh/SkeletalMesh.h"
#include "mesh/SkeletalPose.h"
#include "mesh/AnimationClip.h"
#include "scene/SceneRenderer.h"

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
		}
	}

	AABB SkeletalMeshComponent::GetBoundingBox() const
	{
		return mSkeletalMesh ? mSkeletalMesh->GetBoundingBox() : AABB();
	}

	void SkeletalMeshComponent::OnRender(SceneRenderer *renderer)
	{
		if (mSkeletalMesh)
			renderer->SubmitSkeletalMesh(mSkeletalMesh, GetWorldTransform(), GetBoneTransforms(),
										 mOverrideMaterials);
	}

	const std::vector<glm::mat4> &SkeletalMeshComponent::GetBoneTransforms() const
	{
		if (mPose)
		{
			return mPose->GetTransformsJointSpace();
		}

		return mSkeletalMesh->GetDefaultPose()->GetTransformsJointSpace();
	}

	void SkeletalMeshComponent::Serialize(StreamWriter &ar) const
	{
		MeshComponent::Serialize(ar);
		ar(mSkeletalMesh);
	}

	void SkeletalMeshComponent::Deserialize(StreamReader &ar)
	{
		MeshComponent::Deserialize(ar);
		ar(mSkeletalMesh);
		SetSkeletalMesh(mSkeletalMesh);
	}

	
	REFLECT(SkeletalMeshComponent)
	{
		BEGIN_REFLECT(SkeletalMeshComponent)
		(META_DATA(ClassMetaData_ComponentSpawnable, true)) REQUIRED_COMPONENT_FUNCS()
			REFLECT_PROPERTY("SkeletalMesh", GetSkeletalMesh, SetSkeletalMesh);
	}
}