#pragma once

#include "Material.h"

namespace BHive
{
	class BHIVE_API StandardMaterial : public Material
	{
	public:
		enum EFlags : uint16_t
		{
			None = 0,
			CastShadows = BIT(0),
			ReceiveShadows = BIT(1),
			DiaElectric = BIT(2),
			Shadows = CastShadows | ReceiveShadows
		};

	public:
		FColor Albedo{1.0f, 1.0f, 1.0f, 1.0f};

		FColor Emission{0.0f, 0.0f, 0.0f};

		float Metallic{0.0f};

		float Roughness{1.0f};

		float Opacity{1.0f};

		float DepthScale{1.0f};

		glm::vec2 Tiling{1.0f, 1.0f};

		TEnumAsByte<EFlags> Flags = Shadows;

	public:
		StandardMaterial();

		void Submit(const Ref<Shader> &shader) override;

		Ref<Shader> GetShader() const;

		void Save(cereal::BinaryOutputArchive &ar) const override;

		void Load(cereal::BinaryInputArchive &ar) override;

		virtual bool ShouldCastShadows() const override;

		//Ref<Material> Clone() const override;

		REFLECTABLEV(Material)
	};

	REFLECT_EXTERN(StandardMaterial)

} // namespace BHive