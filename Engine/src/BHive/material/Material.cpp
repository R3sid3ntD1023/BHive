#include "gfx/RenderCommand.h"
#include "gfx/Shader.h"
#include "gfx/ShaderManager.h"
#include "gfx/Texture.h"
#include "Material.h"
#include "renderers/Renderer.h"

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

		mTextures = {{ALBEDO_TEX, nullptr},		  {METALLIC_TEX, nullptr},			{ROUGHNESS_TEX, nullptr},
					 {NORMAL_TEX, nullptr},		  {EMISSION_TEX, nullptr},			{OPACITY_TEX, nullptr},
					 {DISPLACEMENT_TEX, nullptr}, {METALLIC_ROUGHNESS_TEX, nullptr}};
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

		int32_t flags = mFlags;

		for (auto &[name, texture] : mTextures)
		{
			auto &texture_slot = mTextureSlots.at(name);

			auto texture_used = texture ? texture : texture_slot.mTexture;

			if (texture_used.get())
			{
				if (name == METALLIC_ROUGHNESS_TEX)
				{
					flags |= MaterialFlag_Use_Metallic_Roughness;
				}

				else if (name == NORMAL_TEX)
				{
					flags |= MaterialFlag_Use_Normal_Map;
				}

				else if (name == DISPLACEMENT_TEX)
				{
					flags |= MaterialFlag_Use_Depth_Map;
				}

				texture_used->Bind(texture_slot.mBinding);
			}

			shader->SetUniform("u_material.flags", flags);
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
		static Ref<Shader> shader = ShaderManager::Get().Load(ENGINE_PATH "/data/shaders/BDRFMaterial.glsl");
		return shader;
	}

	void Material::Save(cereal::BinaryOutputArchive &ar) const
	{
		Asset::Save(ar);
		// ar(mAldebo, mMetallic, mRoughness, mDiaElectric, mEmission, mOpacity, mTiling, mDepthScale, mFlags, mTextures);
	}

	void Material::Load(cereal::BinaryInputArchive &ar)
	{
		Asset::Load(ar);
		// ar(mAldebo, mMetallic, mRoughness, mDiaElectric, mEmission, mOpacity, mTiling, mDepthScale, mFlags, mTextures);
	}

	bool Material::CastShadows() const
	{
		return (mFlags & MaterialFlag_Cast_Shadows) != 0;
	}

	bool Material::IsTransparent() const
	{
		return (mFlags & MaterialFlag_Transparent) != 0 || mOpacity < 1.0f ||
			   (mFlags & MaterialFlag_Alpha_Is_Transparency) != 0;
	}
} // namespace BHive