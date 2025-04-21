#pragma once

#include "Material.h"

namespace BHive
{
	enum MaterialFlags : uint16_t
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
		MaterialFlag_Shadows = MaterialFlag_Cast_Shadows | MaterialFlag_Recieve_Shadows
	};

	class BDRFMaterial : public Material
	{

	public:
		BDRFMaterial();

		FColor mAldebo{0xFFFFFFFF};

		float mMetallic{0.0f};

		float mRoughness{1.0f};

		float mOpacity{1.f};

		bool mDiaElectric = true;

		FColor mEmission{0xFF000000};

		glm::vec2 mTiling{1.0f};

		float mDepthScale = 1.0f;

		TEnumAsByte<MaterialFlags> mFlags = MaterialFlag_Shadows;

		virtual void Submit() const;

		bool CastShadows() const;

		virtual bool IsTransparent() const;

		Ref<Shader> GetShader() const;

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;

		virtual void Load(cereal::BinaryInputArchive &ar) override;
	};


} // namespace BHive