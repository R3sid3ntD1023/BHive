#include "gfx/RenderCommand.h"
#include "gfx/Shader.h"
#include "gfx/ShaderManager.h"
#include "gfx/Texture.h"
#include "BDRFMaterial.h"
#include "renderers/Renderer.h"
#include "gfx/UniformBuffer.h"

namespace BHive
{
	BDRFMaterial::BDRFMaterial()
		: Material(GetShader())
	{
		mTextureSlots = {{"Albedo", 6},	   {"Metallic", 7}, {"Roughness", 8},	 {"Normal", 9},
						 {"Emission", 10}, {"Opacity", 11}, {"Displacment", 12}, {"MetallicRougness", 13}};

		mTextures = {{"Albedo", Renderer::GetWhiteTexture()},	   {"Metallic", Renderer::GetBlackTexture()},
					 {"Roughness", Renderer::GetWhiteTexture()},   {"Normal", nullptr},
					 {"Emission", Renderer::GetBlackTexture()},	   {"Opacity", Renderer::GetWhiteTexture()},
					 {"Displacment", Renderer::GetBlackTexture()}, {"MetallicRougness", Renderer::GetBlackTexture()}};
	}

	void BDRFMaterial::Submit(const Ref<UniformBuffer> &material_buffer)
	{

		for (auto &slot : mTextureSlots)
		{
			auto texture = mTextures[slot.first];
			if (!texture)
			{

				Renderer::GetWhiteTexture()->Bind(slot.second);
				continue;
			}

			texture->Bind(slot.second);
		}

		material_buffer->SetData(&mMaterialData, sizeof(FBDRFMaterialData), 0);
	}

	Ref<Shader> BDRFMaterial::GetShader() const
	{
		static Ref<Shader> shader = ShaderManager::Get().Load(ENGINE_PATH "/data/shaders/BDRFMaterial.glsl");
		return shader;
	}

	void BDRFMaterial::Save(cereal::BinaryOutputArchive &ar) const
	{
		Material::Save(ar);
		ar(mMaterialData);
	}

	void BDRFMaterial::Load(cereal::BinaryInputArchive &ar)
	{
		Material::Load(ar);
		ar(mMaterialData);
	}

	REFLECT(EMaterialFlags)
	{
		BEGIN_REFLECT_ENUM(EMaterialFlags)(
			ENUM_VALUE(MaterialFlag_None), ENUM_VALUE(MaterialFlag_Show_Vertex_Colors),
			ENUM_VALUE(MaterialFlag_Alpha_Is_Transparency), ENUM_VALUE(MaterialFlag_Use_Metallic_Roughness),
			ENUM_VALUE(MaterialFlag_Use_Normal_Map), ENUM_VALUE(MaterialFlag_Use_Depth_Map),
			ENUM_VALUE(MaterialFlag_Cast_Shadows), ENUM_VALUE(MaterialFlag_Recieve_Shadows), ENUM_VALUE(MaterialFlag_UnLit),
			ENUM_VALUE(MaterialFlag_DoubleSided), ENUM_VALUE(MaterialFlag_Transparent), ENUM_VALUE(MaterialFlag_DiaElectric),
			ENUM_VALUE(MaterialFlag_Shadows));
	}

	REFLECT(BDRFMaterial)
	{
		BEGIN_REFLECT(BDRFMaterial) REFLECT_CONSTRUCTOR() REFLECT_PROPERTY("MaterialData", mMaterialData);
	}

	REFLECT(FBDRFMaterialData)
	{
		BEGIN_REFLECT(FBDRFMaterialData)
		REFLECT_PROPERTY("Albedo", Albedo)
		REFLECT_PROPERTY("Metallic", Metallic)
		REFLECT_PROPERTY("Roughness", Roughness)
		REFLECT_PROPERTY("Emission", Emission)
		REFLECT_PROPERTY("Opacity", Opacity)
		REFLECT_PROPERTY("Tiling", Tiling)
		REFLECT_PROPERTY("DepthScale", DepthScale)
		REFLECT_PROPERTY("Flags", Flags)(META_DATA(EPropertyMetaData_Flags, EPropertyFlags_BitFlags));
	}
} // namespace BHive