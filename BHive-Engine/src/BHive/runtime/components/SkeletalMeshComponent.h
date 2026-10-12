#pragma once

#include "gfx/material/MaterialTable.h"
#include "gfx/mesh/SkeletalMesh.h"
#include "gfx/registries/Handles.h"
#include "runtime/Component.h"

namespace BHive
{
	class SkeletalMesh;

	class BHIVE_API SkeletalMeshComponent : public Component
	{
	public:
		SkeletalMeshComponent() = default;

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;

		virtual void Load(cereal::BinaryInputArchive &ar) override;

		void OnRender(const FComponentRenderContext &context) override;

		void SetSkeletalMesh(AssetHandle<SkeletalMesh> h);

		AssetHandle<SkeletalMesh> GetSkeletalMesh() const { return SkeletalMeshAsset; }

		const MaterialTable &GetMaterials() const { return mOverrideMaterials; }

		REFLECTABLEV(Component)

	private:
		AssetHandle<SkeletalMesh> SkeletalMeshAsset;

		MaterialTable mOverrideMaterials;
	};

	REFLECT_EXTERN(SkeletalMeshComponent)
} // namespace BHive