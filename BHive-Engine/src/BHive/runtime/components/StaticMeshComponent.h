#pragma once

#include "runtime/Component.h"
#include "gfx/mesh/StaticMesh.h"

namespace BHive
{
	struct BHIVE_API StaticMeshComponent : public Component
	{
		StaticMeshComponent() = default;
		StaticMeshComponent(const StaticMeshComponent &other) = default;

		void Save(cereal::BinaryOutputArchive &ar) const override;

		void Load(cereal::BinaryInputArchive &ar) override;

		const Ref<StaticMesh> &GetStaticMesh() const { return mStaticMeshAsset; }

		void SetStaticMesh(const Ref<StaticMesh> &mesh);

		const MaterialTable &GetMaterials() const { return mOverrideMaterials; }

		REFLECTABLEV(Component)

	private:
		Ref<StaticMesh> mStaticMeshAsset;
		MaterialTable mOverrideMaterials;
	};

	REFLECT_EXTERN(StaticMeshComponent)
} // namespace BHive