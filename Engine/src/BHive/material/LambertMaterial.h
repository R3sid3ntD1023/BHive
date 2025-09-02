#pragma once

#include "material/Material.h"

namespace BHive
{

	class LambertMaterial : public Material
	{

	public:
		LambertMaterial();

		FColor DiffuseColor = FColor::White;

		FColor EmissionColor = FColor::Black;

		void Save(cereal::BinaryOutputArchive &ar) const override;

		void Load(cereal::BinaryInputArchive &ar) override;

		void Submit(const Ref<Shader> &shader) override;

		Ref<Material> Clone() const override;

		Ref<Shader> GetShader() const;

		REFLECTABLEV(Material)
	};

	REFLECT_EXTERN(LambertMaterial)
} // namespace BHive