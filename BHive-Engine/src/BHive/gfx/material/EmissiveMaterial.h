#pragma once

#include "Material.h"

namespace BHive
{

	class BHIVE_API EmissiveMaterial : public Material
	{

	public:
		EmissiveMaterial() = default;

		FColor EmissionColor = FColor::Black;

		void Save(cereal::BinaryOutputArchive &ar) const override;

		void Load(cereal::BinaryInputArchive &ar) override;

		void Submit(Pipeline* pipeline = nullptr) override;

		///Ref<Material> Clone() const override;

		REFLECTABLEV(Material)
	};

	REFLECT_EXTERN(EmissiveMaterial)
} // namespace BHive