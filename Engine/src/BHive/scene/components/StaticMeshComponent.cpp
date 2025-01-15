#include "mesh/StaticMesh.h"
#include "scene/SceneRenderer.h"
#include "StaticMeshComponent.h"

namespace BHive
{
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

	void StaticMeshComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		MeshComponent::Save(ar);
		ar(mStaticMesh);
	}

	void StaticMeshComponent::Load(cereal::BinaryInputArchive &ar)
	{
		MeshComponent::Load(ar);
		ar(mStaticMesh);
		SetStaticMesh(mStaticMesh);
	}

	REFLECT(StaticMeshComponent)
	{
		BEGIN_REFLECT(StaticMeshComponent)
		(META_DATA(ClassMetaData_ComponentSpawnable, true)) REQUIRED_COMPONENT_FUNCS()
			REFLECT_PROPERTY("StaticMesh", GetStaticMesh, SetStaticMesh);
	}

} // namespace BHive