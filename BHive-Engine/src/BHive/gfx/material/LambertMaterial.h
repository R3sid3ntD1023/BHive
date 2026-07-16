#pragma once

#include "Material.h"

namespace BHive
{

	class BHIVE_API LambertMaterial : public Material
	{

	public:
		LambertMaterial() = default;

		FColor DiffuseColor = FColor::White;

		FColor EmissionColor = FColor::Black;

		void Save(cereal::BinaryOutputArchive &ar) const override;

		void Load(cereal::BinaryInputArchive &ar) override;

		void Submit(Pipeline *pipeline = nullptr) override;

		// Ref<Material> Clone() const override;

		REFLECTABLEV(Material)
	};

	REFLECT_EXTERN(LambertMaterial)
} // namespace BHive