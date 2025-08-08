#pragma once

#include "material/Material.h"

namespace BHive
{

	class PlanetMaterial : public Material
	{
		enum EFlags : uint16_t
		{
			None = 0,
			AsSingleChannel = BIT(0)
		};

	public:
		PlanetMaterial();

		FColor DiffuseColor = Colors::White;

		FColor EmissionColor = Colors::Black;

		EFlags Flags = EFlags::None;

		void Save(cereal::JSONOutputArchive &ar) const override;

		void Load(cereal::JSONInputArchive &ar) override;

		void Submit(const Ref<Shader> &shader) override;

		Ref<Shader> GetShader() const;

		REFLECTABLEV(Material)
	};
} // namespace BHive