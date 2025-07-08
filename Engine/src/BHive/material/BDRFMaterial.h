#pragma once

#include "Material.h"

namespace BHive
{
	enum EMaterialFlags : uint16_t
	{
		MaterialFlag_None = 0,
		MaterialFlag_Show_Vertex_Colors = BIT(0),
		MaterialFlag_Alpha_Is_Transparency = BIT(1),
		MaterialFlag_Use_Metallic_Roughness = BIT(2),
		MaterialFlag_Use_Normal_Map = BIT(3),
		MaterialFlag_Use_Depth_Map = BIT(4),
		MaterialFlag_Cast_Shadows = BIT(5),
		MaterialFlag_Recieve_Shadows = BIT(6),
		MaterialFlag_UnLit = BIT(7),
		MaterialFlag_DoubleSided = BIT(8),
		MaterialFlag_Transparent = BIT(9),
		MaterialFlag_DiaElectric = BIT(10),
		MaterialFlag_Shadows = MaterialFlag_Cast_Shadows | MaterialFlag_Recieve_Shadows
	};

	struct FBDRFMaterialData
	{
		glm::vec4 Albedo{1.0f, 1.0f, 1.0f, 1.0f};
		alignas(16) glm::vec3 Emission{0.0f, 0.0f, 0.0f};
		alignas(16) glm::vec2 Tiling{1.0f, 1.0f};
		float Metallic{0.0f};
		float Roughness{1.0f};
		float Opacity{1.0f};
		float DepthScale{1.0f};
		EMaterialFlags Flags = MaterialFlag_None;

		template <class Archive>
		void Serialize(Archive &ar)
		{
			ar(Albedo, Emission, Tiling, Metallic, Roughness, Opacity, DepthScale, Flags);
		}
	};

	class BDRFMaterial : public Material
	{

	public:
		BDRFMaterial();

		FBDRFMaterialData mMaterialData;

		void Submit(const Ref<UniformBuffer> &material_buffer) override;

		Ref<Shader> GetShader() const;

		void Save(cereal::BinaryOutputArchive &ar) const override;

		void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLEV(Material)
	};

	REFLECT_EXTERN(BDRFMaterial)
	REFLECT_EXTERN(FBDRFMaterialData)

} // namespace BHive