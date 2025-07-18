#include "StaticMeshComponent.h"
#include "Renderers/MeshRenderer.h"
#include "Renderers/LineRenderer.h"
#include "GameObject.h"

namespace BHive
{
	void StaticMeshComponent::Render()
	{
		if (StaticMeshAsset)
		{
			auto world_transform = GetOwner()->GetWorldTransform();
			MeshRenderer::DrawMesh(StaticMeshAsset, world_transform);
			LineRenderer::DrawAABB(StaticMeshAsset->GetBoundingBox(), Colors::Red, world_transform);
		}
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
