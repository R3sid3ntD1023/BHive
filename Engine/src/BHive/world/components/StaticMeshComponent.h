#pragma once

#include "Component.h"
#include "mesh/StaticMesh.h"

namespace BHive
{
	struct StaticMeshComponent : public Component
	{

		StaticMeshComponent() = default;
		StaticMeshComponent(const StaticMeshComponent &other) = default;

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;

		virtual void Load(cereal::BinaryInputArchive &ar) override;

		virtual void Save(cereal::JSONOutputArchive &ar) const override;

		virtual void Load(cereal::JSONInputArchive &ar) override;

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