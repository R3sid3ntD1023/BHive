#pragma once

#include "gfx/Color.h"
#include "asset/Asset.h"
#include "gfx/Texture.h"
#include "serialization/Serialization.h"
#include "core/EnumAsByte.h"

namespace BHive
{
	class Shader;
	class Texture;

	enum MaterialFlags : uint16_t
	{
		MaterialFlag_None = 0,
		MaterialFlag_UnLit = BIT(0),
		MaterialFlag_DoubleSided = BIT(1),
		MaterialFlag_Transparent = BIT(2),
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

		bool mCastShadows = true;

		bool mReceiveShadows = true;

		bool mShowVertexColors = false;

		bool mAlphaIsTransparency = false;

		TEnumAsByte<MaterialFlags> mFlags = MaterialFlag_None;

		virtual Ref<Shader> Submit() const;

		virtual void SetTexture(const char *name, const Ref<Texture> &texture);

		bool CastShadows() const { return mCastShadows; }
		virtual bool IsTransparent() const { return (mFlags & MaterialFlag_Transparent) != 0 || mOpacity < 1.0f ||
													mAlphaIsTransparency; }

		static Ref<Shader> GetShader();

		void Serialize(StreamWriter &ar) const;
		void Deserialize(StreamReader &ar);

		ASSET_CLASS(Material)
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

}