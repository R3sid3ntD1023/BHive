#pragma once

#include "Material.h"

namespace BHive
{

	class BHIVE_API LambertMaterial : public Material
	{

	public:
		LambertMaterial()
			: Material(ShaderManager::Get("LambertMaterial.glsl"))
		{
			SetParam("DiffuseColor", MaterialParam(mDiffuseColor));
			SetParam("Emission", MaterialParam(mEmissionColor));
		}

		LambertMaterial &SetDiffuseColor(FColor color);

		LambertMaterial &SetEmissionColor(FColor color);

		void Save(cereal::BinaryOutputArchive &ar) const override;

		void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLEV(Material)

	private:
		FColor mDiffuseColor = FColor::White;

		FColor mEmissionColor = FColor::Black;
	};

	REFLECT_EXTERN(LambertMaterial)
} // namespace BHive