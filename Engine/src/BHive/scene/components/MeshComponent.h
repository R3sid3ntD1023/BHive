#pragma once

#include "material/MaterialTable.h"
#include "physics/ColliderComponent.h"

namespace BHive
{
	struct BHIVE MeshComponent : public ShapeComponent
	{

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;

		virtual void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLEV(ShapeComponent)

	protected:
		void SetMaterialTable(const MaterialTable &materials);

	protected:
		MaterialTable mOverrideMaterials;
		MaterialTable mMaterials;
	};

	REFLECT_EXTERN(MeshComponent)
} // namespace BHive