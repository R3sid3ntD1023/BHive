#include "Material.h"
#include "gfx/Shader.h"
#include "gfx/Texture.h"
#include "renderers/Renderer.h"
#include "serialization/StreamReader.h"
#include "serialization/StreamWriter.h"
#include "asset/AssetManager.h"
#include "gfx/RenderCommand.h"

namespace BHive
{
	Material::Material()
	{
		mTextureSlots = {
			{ALBEDO_TEX, {6, Renderer::GetWhiteTexture()}},
			{METALLIC_TEX, {7, Renderer::GetWhiteTexture()}},
			{ROUGHNESS_TEX, {8, Renderer::GetWhiteTexture()}},
			{NORMAL_TEX, {9, nullptr}},
			{EMISSION_TEX, {10, Renderer::GetWhiteTexture()}},
			{OPACITY_TEX, {11, Renderer::GetWhiteTexture()}},
			{DISPLACEMENT_TEX, {12, nullptr}},
			{METALLIC_ROUGHNESS_TEX, {13, nullptr}}};

		mTextures = {
			{ALBEDO_TEX, nullptr},
			{METALLIC_TEX, nullptr},
			{ROUGHNESS_TEX, nullptr},
			{NORMAL_TEX, nullptr},
			{EMISSION_TEX, nullptr},
			{OPACITY_TEX, nullptr},
			{DISPLACEMENT_TEX, nullptr},
			{METALLIC_ROUGHNESS_TEX, nullptr}};
	}

	Ref<Shader> Material::Submit() const
	{
		auto shader = GetShader();
		shader->Bind();
		shader->SetUniform("u_material.Albedo", (glm::vec4)mAldebo);
		shader->SetUniform("u_material.Metallic", mMetallic);
		shader->SetUniform("u_material.Roughness", mRoughness);
		shader->SetUniform("u_material.Opacity", mOpacity);
		shader->SetUniform("u_material.DiaElectric", (int)mDiaElectric);
		shader->SetUniform("u_material.Emission", (glm::vec3)mEmission);
		shader->SetUniform("u_material.Tiling", mTiling);
		shader->SetUniform("u_material.DepthScale", mDepthScale);
		shader->SetUniform("u_material.ReceiveShadows", (int)mReceiveShadows);
		shader->SetUniform("u_material.ShowVertexColors", (int)mShowVertexColors);
		shader->SetUniform("u_material.AlphaIsTranparency", (int)mAlphaIsTransparency);

		for (auto &[name, texture] : mTextures)
		{
			auto &texture_slot = mTextureSlots.at(name);

			auto texture_used = texture ? texture : texture_slot.mTexture;
			if (texture_used)
			{
				texture_used->Bind(texture_slot.mBinding);
			}

			if (name == METALLIC_ROUGHNESS_TEX)
			{
				shader->SetUniform("u_material.MetallicRoughnessCombined", texture_used ? 1 : 0);
			}

			else if (name == NORMAL_TEX)
			{
				shader->SetUniform("u_material.HasNormalMap", texture_used ? 1 : 0);
			}

			else if (name == DISPLACEMENT_TEX)
			{
				shader->SetUniform("u_material.HasDepthMap", texture_used ? 1 : 0);
			}
		}

		if ((mFlags & MaterialFlag_DoubleSided) != 0)
		{
			RenderCommand::SetCullEnabled(false);
		}
		else
		{
			RenderCommand::SetCullEnabled(true);
		}

		return shader;
	}

	void Material::SetTexture(const char *name, const Ref<Texture> &texture)
	{
		if (mTextures.contains(name))
		{
			mTextures[name] = texture;
		}
	}

	Ref<Shader> Material::GetShader()
	{
		static Ref<Shader> shader = ShaderLibrary::Load(ENGINE_PATH "/data/shaders/BDRFMaterial.glsl");
		return shader;
	}

	void Material::Serialize(StreamWriter &ar) const
	{
		ar(mAldebo, mMetallic, mRoughness, mDiaElectric, mEmission, mOpacity, mTiling, mDepthScale, mFlags, mCastShadows, mReceiveShadows, mAlphaIsTransparency, mTextures);
	}

	void Material::Deserialize(StreamReader &ar)
	{
		ar(mAldebo, mMetallic, mRoughness, mDiaElectric, mEmission, mOpacity, mTiling, mDepthScale, mFlags, mCastShadows, mReceiveShadows, mAlphaIsTransparency, mTextures);
	}

	REFLECT(MaterialFlags)
	{
		BEGIN_REFLECT_ENUM(MaterialFlags)
		(

			ENUM_VALUE(MaterialFlag_None),
			ENUM_VALUE(MaterialFlag_DoubleSided),
			ENUM_VALUE(MaterialFlag_Transparent),
			ENUM_VALUE(MaterialFlag_UnLit));
	}

	REFLECT(Material)
	{
		BEGIN_REFLECT(Material)
		REFLECT_PROPERTY("Albedo", mAldebo)
		REFLECT_PROPERTY("Metallic", mMetallic)
		(META_DATA(EPropertyMetaData_Min, 0.0f), META_DATA(EPropertyMetaData_Max, 1.f), META_DATA(EPropertyMetaData_Flags, EPropertyFlags_Slider))
			REFLECT_PROPERTY("Roughness", mRoughness)(META_DATA(EPropertyMetaData_Min, 0.0f), META_DATA(EPropertyMetaData_Max, 1.f), META_DATA(EPropertyMetaData_Flags, EPropertyFlags_Slider))
				REFLECT_PROPERTY("DiaElectric", mDiaElectric)
					REFLECT_PROPERTY("Emission", mEmission)
						REFLECT_PROPERTY("Opacity", mOpacity)(META_DATA(EPropertyMetaData_Min, 0.0f), META_DATA(EPropertyMetaData_Max, 1.f), META_DATA(EPropertyMetaData_Flags, EPropertyFlags_Slider))
							REFLECT_PROPERTY("Tiling", mTiling)
								REFLECT_PROPERTY("DepthScale", mDepthScale)
									REFLECT_PROPERTY("Flags", mFlags)
										REFLECT_PROPERTY("CastShadows", mCastShadows)
											REFLECT_PROPERTY("RecieveShadows", mReceiveShadows)
												REFLECT_PROPERTY("AlphaIsTransparency", mAlphaIsTransparency)
													REFLECT_PROPERTY("Textures", mTextures)(META_DATA(EPropertyMetaData_Flags, EPropertyFlags_FixedSize));
	}
}