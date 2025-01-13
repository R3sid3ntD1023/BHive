#include "MeshComponent.h"

namespace BHive
{
	void MeshComponent::Save(cereal::JSONOutputArchive &ar) const
	{
		ShapeComponent::Save(ar);
		ar(MAKE_NVP("Materials", mMaterials), MAKE_NVP("OverrideMaterials", mOverrideMaterials));
	}

	void MeshComponent::Load(cereal::JSONInputArchive &ar)
	{
		ShapeComponent::Load(ar);
		ar(MAKE_NVP("Materials", mMaterials), MAKE_NVP("OverrideMaterials", mOverrideMaterials));
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