#include "StaticMeshComponent.h"
#include "Renderers/MeshRenderer.h"
#include "GameObject.h"

namespace BHive
{
	void StaticMeshComponent::Render()
	{
		MeshRenderer::DrawMesh(StaticMeshAsset, GetOwner()->GetWorldTransform());
	}

	void StaticMeshComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		ar(TAssetHandle(StaticMeshAsset));
	}

	void StaticMeshComponent::Load(cereal::BinaryInputArchive &ar)
	{
		ar(TAssetHandle(StaticMeshAsset));
	}

	REFLECT(StaticMeshComponent)
	{
		BEGIN_REFLECT(StaticMeshComponent)(META_DATA(ClassMetaData_ComponentSpawnable, true)) REFLECT_CONSTRUCTOR()
			REFLECT_PROPERTY(StaticMeshAsset) COMPONENT_IMPL();
	}

} // namespace BHive
