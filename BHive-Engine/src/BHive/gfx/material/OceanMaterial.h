#pragma once

#include "Material.h"

namespace BHive
{
	class BHIVE_API OceanMaterial : public Material
	{

	public:
		OceanMaterial()
			: Material("OceanMaterial.glsl")
		{
			SetTexture("ShoreMap", {nullptr});
			SetParam("Time", 0.0f);
			SetParam("ShoreCenter", glm::vec2{0, 0});
			SetParam("ShoreExtent", 0.f);
		}

		void SetTextureFromType(EMaterialTextureType type, Ref<Texture> texture) override {};

		std::vector<std::string> GetShaderDefinitions() const override { return {"MATERIAL_OCEAN"}; }

		void Save(cereal::BinaryOutputArchive &ar) const override {};

		void Load(cereal::BinaryInputArchive &ar) override {};

		REFLECTABLEV(Material)
	};

	REFLECT_EXTERN(OceanMaterial)
} // namespace BHive