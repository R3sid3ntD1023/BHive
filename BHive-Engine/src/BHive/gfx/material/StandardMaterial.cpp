#include "gfx/RenderCommand.h"
#include "gfx/Shader.h"
#include "gfx/ShaderManager.h"
#include "gfx/Texture.h"
#include "gfx/renderers/Renderer.h"
#include "StandardMaterial.h"

namespace BHive
{
	StandardMaterial::StandardMaterial()
		: Material("StandardMaterial.glsl")
	{
		SetAlbedo(FColor::White);
		SetEmission(FColor::Black);
		SetMetalness(0.0f);
		SetRoughness(0.1f);
		SetOpacity(1.0f);
		SetDepthScale(1.0f);
		SetTiling({1.0f, 1.0f});
		SetFlags(EFlags::Shadows);
	}

	StandardMaterial &StandardMaterial::SetAlbedo(FColor color)
	{
		mAlbedo = color;
		SetParam("Albedo", MaterialParam(color));
		return *this;
	}

	StandardMaterial &StandardMaterial::SetEmission(FColor color)
	{
		mEmission = color;
		SetParam("Emission", MaterialParam(color));
		return *this;
	}

	StandardMaterial &StandardMaterial::SetMetalness(float metalness)
	{
		mMetalness = metalness;
		SetParam("Metalness", MaterialParam(metalness));
		return *this;
	}

	StandardMaterial &StandardMaterial::SetRoughness(float roughness)
	{
		mRoughness = roughness;
		SetParam("Roughness", MaterialParam(roughness));
		return *this;
	}

	StandardMaterial &StandardMaterial::SetOpacity(float opacity)
	{
		mOpacity = opacity;
		SetParam("Opacity", MaterialParam(opacity));
		return *this;
	}

	StandardMaterial &StandardMaterial::SetDepthScale(float depthScale)
	{
		mDepthScale = depthScale;
		// SetParam("DepthScale", MaterialParam(depthScale));
		return *this;
	}

	StandardMaterial &StandardMaterial::SetTiling(glm::vec2 tiling)
	{
		mTiling = tiling;
		SetParam("Tiling", MaterialParam(tiling));
		return *this;
	}

	StandardMaterial &StandardMaterial::SetFlags(EFlags flags)
	{
		mFlags = flags;
		SetParam("Flags", MaterialParam((uint32_t)flags));
		return *this;
	}

	IMaterial &StandardMaterial::SetTexture(const std::string &name, const FTextureBinding &texture) &
	{
		Material::SetTexture(name, texture);
		if (name == "NormalMap")
		{
			SetParam("HasNormalMap", MaterialParam(texture.TextureRef != nullptr));
		}
		return *this;
	}

	void StandardMaterial::Save(cereal::BinaryOutputArchive &ar) const
	{
		Material::Save(ar);
		ar(mAlbedo, mEmission, mMetalness, mRoughness, mOpacity, mDepthScale, mTiling, mFlags);
	}

	void StandardMaterial::Load(cereal::BinaryInputArchive &ar)
	{
		Material::Load(ar);
		ar(mAlbedo, mEmission, mMetalness, mRoughness, mOpacity, mDepthScale, mTiling, mFlags);
	}

	bool StandardMaterial::ShouldCastShadows() const
	{
		return (mFlags & CastShadows) != 0;
	}

	REFLECT(StandardMaterial::EFlags)
	{
		BEGIN_REFLECT_ENUM(StandardMaterial::EFlags)(ENUM_VALUE(CastShadows), ENUM_VALUE(ReceiveShadows), ENUM_VALUE(DiaElectric), ENUM_VALUE(Shadows));
	}

	REFLECT(StandardMaterial)
	{
		BEGIN_REFLECT(StandardMaterial)
		REFLECT_CONSTRUCTOR()
		REFLECT_PROPERTY("Albedo", mAlbedo)
		REFLECT_PROPERTY("Metallic", mMetalness)(META_DATA(EPropertyMetaData_Max, 1.0f))(META_DATA(EPropertyMetaData_Min, 0.0f))REFLECT_PROPERTY("Roughness", mRoughness)(
			META_DATA(EPropertyMetaData_Max, 1.0f))(META_DATA(EPropertyMetaData_Min, 0.0f))REFLECT_PROPERTY("Emission", mEmission)(META_DATA(EPropertyMetaData_HDR, true))
			REFLECT_PROPERTY("Opacity", mOpacity)(META_DATA(EPropertyMetaData_Max, 1.0f))(META_DATA(EPropertyMetaData_Min, 0.0f))REFLECT_PROPERTY("Tiling", mTiling)
				REFLECT_PROPERTY("DepthScale", mDepthScale) REFLECT_PROPERTY("Flags", mFlags)(META_DATA(EPropertyFlags_BitFlags, true));

		rttr::type::register_wrapper_converter_for_base_classes<Ref<StandardMaterial>>();
	}

} // namespace BHive