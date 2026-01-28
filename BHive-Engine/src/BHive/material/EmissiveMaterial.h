#pragma once

#include "material/Material.h"

namespace BHive
{

	class BHIVE_API EmissiveMaterial : public Material
	{

	public:
		EmissiveMaterial();

		FColor EmissionColor = FColor::Black;

		void Save(cereal::BinaryOutputArchive &ar) const override;

		void Load(cereal::BinaryInputArchive &ar) override;

		void Submit(const Ref<Shader> &shader) override;

		///Ref<Material> Clone() const override;

		Ref<Shader> GetShader() const;

		REFLECTABLEV(Material)
	};

	REFLECT_EXTERN(EmissiveMaterial)
} // namespace BHive