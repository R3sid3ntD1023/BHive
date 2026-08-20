#pragma once

#include "Material.h"

namespace BHive
{

	class BHIVE_API EmissiveMaterial : public Material
	{

	public:
		EmissiveMaterial();

		IMaterial &SetEmissionColor(FColor color);

		void Save(cereal::BinaryOutputArchive &ar) const override;

		void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLEV(Material)

	private:
		FColor mEmissionColor = FColor::Black;
	};

	REFLECT_EXTERN(EmissiveMaterial)
} // namespace BHive