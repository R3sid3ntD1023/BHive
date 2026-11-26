#pragma once

#include "Material.h"

namespace BHive
{
	class BHIVE_API TerrainMaterial : public Material
	{

	public:
		TerrainMaterial();

		void SetTextureFromType(EMaterialTextureType type, Ref<Texture> texture) override {};

		std::vector<std::string> GetShaderDefinitions() const override { return {"MATERIAL_TERRAIN"}; }

		void Save(cereal::BinaryOutputArchive &ar) const override;

		void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLEV(Material)
	};

	REFLECT_EXTERN(TerrainMaterial)
} // namespace BHive