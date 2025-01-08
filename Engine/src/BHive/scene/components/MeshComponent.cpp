#include "MeshComponent.h"

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

		mMaterials = materials;
		mOverrideMaterials = materials;
	}

		
	REFLECT(MeshComponent)
	{
		BEGIN_REFLECT(MeshComponent)
		REFLECT_PROPERTY("Materials", mOverrideMaterials);
	}
}