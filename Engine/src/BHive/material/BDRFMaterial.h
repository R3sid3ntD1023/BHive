#pragma once

#include "Material.h"

namespace BHive
{
	enum EMaterialFlags : uint16_t
	{
		MaterialFlag_None = 0,
		MaterialFlag_Use_Normal_Map = BIT(0),
		MaterialFlag_DiaElectric = BIT(1),
		MaterialFlag_Cast_Shadows = BIT(2),
		MaterialFlag_Recieve_Shadows = BIT(3),
		MaterialFlag_DoubleSided = BIT(4),

		MaterialFlag_Shadows = MaterialFlag_Cast_Shadows | MaterialFlag_Recieve_Shadows
	};

	class BHIVE_API BDRFMaterial : public Material
	{
	public:
		FColor Albedo{1.0f, 1.0f, 1.0f, 1.0f};

		FColor Emission{0.0f, 0.0f, 0.0f};

		float Metallic{0.0f};

		float Roughness{1.0f};

		float Opacity{1.0f};

		float DepthScale{1.0f};

		glm::vec2 Tiling{1.0f, 1.0f};

		TEnumAsByte<EMaterialFlags> Flags = MaterialFlag_None;

	public:
		BDRFMaterial();

		void Submit(const Ref<Shader> &shader) override;

		Ref<Shader> GetShader() const;

		void Save(cereal::BinaryOutputArchive &ar) const override;

		void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLEV(Material)
	};

	REFLECT_EXTERN(BDRFMaterial)

} // namespace BHive