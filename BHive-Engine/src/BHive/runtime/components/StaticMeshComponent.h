#pragma once

#include "asset/AssetHandle.h"
#include "gfx/material/MaterialTable.h"
#include "gfx/renderers/RenderData.h"
#include "runtime/Component.h"

namespace BHive
{
	class StaticMesh;
	class Material;

	struct BHIVE_API StaticMeshComponent : public Component
	{
		StaticMeshComponent() = default;
		StaticMeshComponent(const StaticMeshComponent &other) = default;

		void Save(cereal::BinaryOutputArchive &ar) const override;

		void Load(cereal::BinaryInputArchive &ar) override;

		void OnRender(const FComponentRenderContext &context) override;

		AssetHandle<StaticMesh> GetStaticMesh() const { return mStaticMeshAsset; }

		void SetStaticMesh(AssetHandle<StaticMesh> mesh);

		void SetMaterial(uint32_t index, AssetHandle<Material> material);

		const MaterialTable &GetMaterials() const { return mOverrideMaterials; }

		REFLECTABLEV(Component)

	private:
		AssetHandle<StaticMesh> mStaticMeshAsset;

		MaterialTable mOverrideMaterials;

		ContextHandle mRenderContext;

		bool mHasSubmittedMesh = false;

		bool mHasSubmittedMaterials = false;
	};

	REFLECT_EXTERN(StaticMeshComponent)
} // namespace BHive