#include "StaticMeshComponent.h"
#include "mesh/StaticMesh.h"
#include "mesh/SkeletalMesh.h"
#include "mesh/SkeletalPose.h"
#include "mesh/AnimationClip.h"
#include "asset/AssetManager.h"
#include "Animator/Animator.h"
#include "material/MaterialTable.h"
#include "scene/SceneRenderer.h"
#include "scene/Actor.h"

namespace BHive
{
	void MeshComponent::Serialize(StreamWriter &ar) const
	{
		ShapeComponent::Serialize(ar);
		ar(mMaterials, mOverrideMaterials);
	}

	void MeshComponent::Deserialize(StreamReader &ar)
	{
		ShapeComponent::Deserialize(ar);
		ar(mMaterials, mOverrideMaterials);
	}

	void MeshComponent::SetMaterialTable(const MaterialTable &materials)
	{
		auto num_materials = mMaterials.size();

		// mOverrideMaterials.resize(materials.size());
		// for (size_t i = 0; i < num_materials; i++)
		// {
		// 	if (i < mMaterials.size())
		// 	{
		// 		if (mOverrideMaterials[i].get() == nullptr)
		// 		{
		// 			mOverrideMaterials[i] = materials[i];
		// 		}
		// 	}
		// 	else
		// 	{
		// 		mOverrideMaterials[i] = materials[i];
		// 	}
		// }

		mMaterials = materials;
		mOverrideMaterials = materials;
	}

	StaticMeshComponent::StaticMeshComponent(const TAssetHandle<StaticMesh> &StaticMesh)
	{
		SetStaticMesh(StaticMesh);
	}

	void StaticMeshComponent::SetStaticMesh(const TAssetHandle<StaticMesh> &StaticMesh)
	{
		if (StaticMesh)
		{

			SetMaterialTable(StaticMesh->GetMaterialTable());
		}

		mStaticMesh = StaticMesh;
	}

	AABB StaticMeshComponent::GetBoundingBox() const
	{
		return mStaticMesh ? mStaticMesh.get()->GetBoundingBox() : AABB();
	}

	void StaticMeshComponent::OnRender(SceneRenderer *renderer)
	{
		renderer->SubmitStaticMesh(mStaticMesh, GetWorldTransform(), mOverrideMaterials);
	}

	void StaticMeshComponent::Serialize(StreamWriter &ar) const
	{
		MeshComponent::Serialize(ar);
		ar(mStaticMesh);
	}

	void StaticMeshComponent::Deserialize(StreamReader &ar)
	{
		MeshComponent::Deserialize(ar);
		ar(mStaticMesh);
		SetStaticMesh(mStaticMesh);
	}

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

	REFLECT(MeshComponent)
	{
		BEGIN_REFLECT(MeshComponent)
		REFLECT_PROPERTY("Materials", mOverrideMaterials);
	}

	REFLECT(StaticMeshComponent)
	{
		BEGIN_REFLECT(StaticMeshComponent)
		(META_DATA(ClassMetaData_ComponentSpawnable, true))
			REQUIRED_COMPONENT_FUNCS()
				REFLECT_PROPERTY("StaticMesh", GetStaticMesh, SetStaticMesh);
	}

	REFLECT(SkeletalMeshComponent)
	{
		BEGIN_REFLECT(SkeletalMeshComponent)
		(META_DATA(ClassMetaData_ComponentSpawnable, true))
			REQUIRED_COMPONENT_FUNCS()
				REFLECT_PROPERTY("SkeletalMesh", GetSkeletalMesh, SetSkeletalMesh);
	}

}