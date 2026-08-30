#pragma once

#include "runtime/Component.h"
#include "gfx/mesh/SkeletalMesh.h"
#include "gfx/registries/Handles.h"

namespace BHive
{
	class SkeletalMesh;

	class BHIVE_API SkeletalMeshComponent : public Component
	{
	public:
		SkeletalMeshComponent() = default;

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;

		virtual void Load(cereal::BinaryInputArchive &ar) override;

		void SetSkeletalMesh(MeshPtr h);

		MeshPtr GetSkeletalMesh() const { return SkeletalMeshAsset; }

		const MaterialTable &GetMaterials() const { return mOverrideMaterials; }

		REFLECTABLEV(Component)

	private:
		MeshPtr SkeletalMeshAsset;

		MaterialTable mOverrideMaterials;
	};

	REFLECT_EXTERN(SkeletalMeshComponent)
} // namespace BHive