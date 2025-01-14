#pragma once

#include "physics/ColliderComponent.h"
#include "material/MaterialTable.h"

namespace BHive
{
	struct BHIVE MeshComponent : public ShapeComponent
	{

		virtual void Save(cereal::JSONOutputArchive &ar) const override;

		virtual void Load(cereal::JSONInputArchive &ar) override;

		REFLECTABLEV(ShapeComponent)

	protected:
		void SetMaterialTable(const MaterialTable &materials);

	protected:
		MaterialTable mOverrideMaterials;
		MaterialTable mMaterials;
	};

	REFLECT_EXTERN(MeshComponent)
} // namespace BHive