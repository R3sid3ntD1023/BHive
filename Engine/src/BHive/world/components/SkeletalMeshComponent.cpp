#include "SkeletalMeshComponent.h"
#include "mesh/SkeletalMesh.h"
#include "World/GameObject.h"
#include "Renderers/Renderer.h"

namespace BHive
{
	void SkeletalMeshComponent::Render()
	{

		if (SkeletalMeshAsset)
		{
			auto pose = SkeletalMeshAsset->GetDefaultPose();
			auto owner = GetOwner();
			auto tranform = owner ? owner->GetWorldTransform() : FTransform{};
			MeshRenderer::DrawMesh(SkeletalMeshAsset, *pose, tranform);
		}
	}

	void SkeletalMeshComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		ar(TAssetHandle(SkeletalMeshAsset));
	}

	void SkeletalMeshComponent::Load(cereal::BinaryInputArchive &ar)
	{
		ar(TAssetHandle(SkeletalMeshAsset));
	}

	REFLECT(SkeletalMeshComponent)
	{
		BEGIN_REFLECT(SkeletalMeshComponent)
		(META_DATA(EClassMetaData::ClassMetaData_ComponentSpawnable, true)) REFLECT_CONSTRUCTOR()
			REFLECT_PROPERTY("Skeletal Mesh Asset", SkeletalMeshAsset) COMPONENT_IMPL();
	}
} // namespace BHive