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

		void Save(cereal::BinaryOutputArchive &ar) const override;

		void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLEV(Material)

	private:
		FColor mDiffuseColor = FColor::White;

		FColor mEmissionColor = FColor::Black;
	};

	REFLECT_EXTERN(LambertMaterial)
} // namespace BHive