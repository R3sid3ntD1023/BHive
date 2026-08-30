#pragma once

#include "runtime/Component.h"
#include "gfx/registries/Handles.h"
#include "gfx/material/MaterialTable.h"

namespace BHive
{
	struct BHIVE_API StaticMeshComponent : public Component
	{
		StaticMeshComponent() = default;
		StaticMeshComponent(const StaticMeshComponent &other) = default;

		void Save(cereal::BinaryOutputArchive &ar) const override;

		void Load(cereal::BinaryInputArchive &ar) override;

		MeshPtr GetStaticMesh() const { return mStaticMeshAsset; }

		void SetStaticMesh(MeshPtr mesh);

		const MaterialTable &GetMaterials() const { return mOverrideMaterials; }

		REFLECTABLEV(Component)

	private:
		MeshPtr mStaticMeshAsset;
		MaterialTable mOverrideMaterials;
	};

	REFLECT_EXTERN(StaticMeshComponent)
} // namespace BHive