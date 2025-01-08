#pragma once

#include "physics/ColliderComponent.h"
#include "material/MaterialTable.h"

namespace BHive
{
	struct BHIVE MeshComponent : public ShapeComponent
	{
		
		void Serialize(StreamWriter& ar) const;
		void Deserialize(StreamReader& ar);

		REFLECTABLEV(ShapeComponent)

	protected:
		void SetMaterialTable(const MaterialTable& materials);

	protected:
		MaterialTable mOverrideMaterials;
		MaterialTable mMaterials;
	};

	REFLECT_EXTERN(MeshComponent)
}