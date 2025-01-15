#include "MeshComponent.h"

namespace BHive
{
	void MeshComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		ShapeComponent::Save(ar);
		ar(mMaterials, mOverrideMaterials);
	}

	void MeshComponent::Load(cereal::BinaryInputArchive &ar)
	{
		ShapeComponent::Load(ar);
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
} // namespace BHive