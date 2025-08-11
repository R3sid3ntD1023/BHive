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
		AddTextureSlot("Albedo", 0);
		AddTextureSlot("Normal", 1);
		AddTextureSlot("Roughness", 2);
		AddTextureSlot("Metallic", 3);
		AddTextureSlot("Emission", 4);
		AddTextureSlot("Opacity", 5);
	}

	void BDRFMaterial::Submit(const Ref<Shader> &shader)
	{

		Material::Submit(shader);

		auto flags = Flags;
		if (mTextures.at("Normal").Texture)
			flags |= EMaterialFlags::MaterialFlag_Use_Normal_Map;

		shader->SetUniform<glm::vec3>("constants.Albedo", Albedo);
		shader->SetUniform<glm::vec3>("constants.Emission", Emission);
		shader->SetUniform("constants.Roughness", Roughness);
		shader->SetUniform("constants.Metalness", Metallic);
		shader->SetUniform("constants.Opacity", Opacity);
		shader->SetUniform("constants.Tiling", Tiling);
		shader->SetUniform("constants.Flags", (uint32_t)flags);
	}

	Ref<Shader> BDRFMaterial::GetShader() const
	{
		static Ref<Shader> shader = ShaderManager::Get().Load(ENGINE_SHADER_PATH "/BDRFMaterial.glsl");
		return shader;
	}

	void BDRFMaterial::Save(cereal::BinaryOutputArchive &ar) const
	{
		Material::Save(ar);
		ar(Albedo, Emission, Metallic, Roughness, Opacity, DepthScale, Tiling, Flags);
	}

	void BDRFMaterial::Load(cereal::BinaryInputArchive &ar)
	{
		Material::Load(ar);
		ar(Albedo, Emission, Metallic, Roughness, Opacity, DepthScale, Tiling, Flags);
	}

	REFLECT(EMaterialFlags)
	{
		BEGIN_REFLECT_ENUM(EMaterialFlags)(
			ENUM_VALUE(MaterialFlag_Cast_Shadows), ENUM_VALUE(MaterialFlag_Recieve_Shadows), ENUM_VALUE(MaterialFlag_DoubleSided), ENUM_VALUE(MaterialFlag_DiaElectric),
			ENUM_VALUE(MaterialFlag_Shadows));
	}

	REFLECT(BDRFMaterial)
	{
		BEGIN_REFLECT(BDRFMaterial)
		REFLECT_CONSTRUCTOR()
		REFLECT_PROPERTY("Albedo", Albedo)
		REFLECT_PROPERTY("Metallic", Metallic)(META_DATA(EPropertyMetaData_Max, 1.0f))(META_DATA(EPropertyMetaData_Min, 0.0f))REFLECT_PROPERTY("Roughness", Roughness)(
			META_DATA(EPropertyMetaData_Max, 1.0f))(META_DATA(EPropertyMetaData_Min, 0.0f))REFLECT_PROPERTY("Emission", Emission)(META_DATA(EPropertyMetaData_HDR, true))
			REFLECT_PROPERTY("Opacity", Opacity)(META_DATA(EPropertyMetaData_Max, 1.0f))(META_DATA(EPropertyMetaData_Min, 0.0f))REFLECT_PROPERTY("Tiling", Tiling)
				REFLECT_PROPERTY("DepthScale", DepthScale) REFLECT_PROPERTY("Flags", Flags)(META_DATA(EPropertyFlags_BitFlags, true));

		rttr::type::register_wrapper_converter_for_base_classes<Ref<BDRFMaterial>>();
	}
} // namespace BHive