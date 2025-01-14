#pragma once

#include "gfx/Color.h"
#include "gfx/Texture.h"
#include "core/EnumAsByte.h"
#include "asset/Asset.h"

namespace BHive
{
	class Shader;
	class Texture;

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

	struct BHIVE FTextureSlot
	{
		uint32_t mBinding = 0;
		TAssetHandle<Texture> mTexture;
	};

	class Material : public Asset
	{
	public:
		using TextureSlots = std::unordered_map<std::string, FTextureSlot>;
		using Textures = std::unordered_map<std::string, TAssetHandle<Texture>>;

	public:
		Material();

		Color mAldebo{0xFFFFFFFF};

		float mMetallic{0.0f};

		float mRoughness{1.0f};

		float mOpacity{1.f};

		bool mDiaElectric = true;

		Color mEmission{0xFF000000};

		glm::vec2 mTiling{1.0f};

		float mDepthScale = 1.0f;

		TEnumAsByte<MaterialFlags> mFlags = MaterialFlag_Shadows;

		virtual Ref<Shader> Submit() const;

		virtual void SetTexture(const char *name, const Ref<Texture> &texture);

		bool CastShadows() const;

		virtual bool IsTransparent() const;

		static Ref<Shader> GetShader();

		virtual void Save(cereal::JSONOutputArchive &ar) const override;

		virtual void Load(cereal::JSONInputArchive &ar) override;

		REFLECTABLEV(Asset)

	private:
		TextureSlots mTextureSlots;
		Textures mTextures;
	};

	REFLECT_EXTERN(MaterialFlags)
	REFLECT_EXTERN(Material)

#define ALBEDO_TEX "u_albedo_map"
#define METALLIC_TEX "u_metallic_map"
#define ROUGHNESS_TEX "u_roughness_map"
#define EMISSION_TEX "u_emission_map"
#define OPACITY_TEX "u_opacity_map"
#define NORMAL_TEX "u_normal_map"
#define DISPLACEMENT_TEX "u_depth_map"
#define METALLIC_ROUGHNESS_TEX "u_metallic_roughness_map"

} // namespace BHive