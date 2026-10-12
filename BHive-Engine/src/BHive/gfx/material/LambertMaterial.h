#pragma once

#include "Material.h"

namespace BHive
{

	class BHIVE_API LambertMaterial : public Material
	{

	public:
		LambertMaterial();

		LambertMaterial &SetDiffuseColor(FColor color);

		LambertMaterial &SetEmissionColor(FColor color);

		void SetTextureFromType(EMaterialTextureType type, Ref<Texture> texture) override;

		std::vector<std::string> GetShaderDefinitions() const override { return {"MATERIAL_LAMBERT"}; }

		void Save(cereal::BinaryOutputArchive &ar) const override;

		void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLEV(Material)

	private:
		FColor mDiffuseColor = FColor::White;

		FColor mEmissionColor = FColor::Black;
	};

	REFLECT_EXTERN(LambertMaterial)
} // namespace BHive