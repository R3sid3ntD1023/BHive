#pragma once

#include "runtime/Component.h"
#include "asset/AssetHandle.h"
#include "runtime/Terrain.h"
#include "gfx/renderers/RenderData.h"
#include "gfx/mesh/StaticMesh.h"

namespace BHive
{
	class Material;

	struct BHIVE_API TerrainComponent : public Component
	{
		TerrainComponent() = default;
		TerrainComponent(const TerrainComponent &) = default;

		FTerrainParameters TerrainParameters{};

		void Build();

		void SetMaterial(AssetHandle<Material> material);

		AssetHandle<Material> GetMaterial() const { return mMaterial.Get(0); }

		void OnRender(const FComponentRenderContext &context) override;

		void Save(cereal::BinaryOutputArchive &ar) const override;

		void Load(cereal::BinaryInputArchive &ar) override;

		Terrain &GetTerrain() { return mTerrain; }

		REFLECTABLEV(Component)

	private:
		Terrain mTerrain;
		MaterialTable mMaterial{1};
		Ref<StaticMesh> mTerrainMesh;
		ContextHandle mRenderContext{};
		bool mIsMeshDirty = true;
		bool mIsMaterialDirty = true;
	};

	REFLECT_EXTERN(TerrainComponent)
} // namespace BHive