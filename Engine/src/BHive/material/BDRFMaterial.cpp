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
		AddTextureSlot("Albedo", 6);
		AddTextureSlot("Metallic", 7);
		AddTextureSlot("Roughness", 8);
		AddTextureSlot("Normal", 9);
		AddTextureSlot("Emission", 10);
		AddTextureSlot("Opacity", 11);
		AddTextureSlot("Displacment", 12);
		AddTextureSlot("MetallicRougness", 13);
	}

	void BDRFMaterial::Submit(const Ref<Shader> &shader)
	{

		Material::Submit(shader);

		shader->SetUniform<glm::vec4>("constants.u_material.Albedo", Albedo);
		shader->SetUniform("constants.u_material.Metallic", Metallic);
		shader->SetUniform("constants.u_material.Roughness", Roughness);
		shader->SetUniform<glm::vec3>("constants.u_material.Emission", Emission);
		shader->SetUniform("constants.u_material.Opacity", Opacity);
		shader->SetUniform("constants.u_material.Tiling", Tiling);
		shader->SetUniform("constants.u_material.DepthScale", DepthScale);
		shader->SetUniform("constants.u_material.Flags", (int32_t)Flags);
	}

	Ref<Shader> BDRFMaterial::GetShader() const
	{
		static Ref<Shader> shader = ShaderManager::Get().Load(ENGINE_PATH "/data/shaders/BDRFMaterial.glsl");
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
			ENUM_VALUE(MaterialFlag_Show_Vertex_Colors), ENUM_VALUE(MaterialFlag_Alpha_Is_Transparency),
			ENUM_VALUE(MaterialFlag_Use_Metallic_Roughness), ENUM_VALUE(MaterialFlag_Use_Normal_Map),
			ENUM_VALUE(MaterialFlag_Use_Depth_Map), ENUM_VALUE(MaterialFlag_Cast_Shadows),
			ENUM_VALUE(MaterialFlag_Recieve_Shadows), ENUM_VALUE(MaterialFlag_UnLit), ENUM_VALUE(MaterialFlag_DoubleSided),
			ENUM_VALUE(MaterialFlag_Transparent), ENUM_VALUE(MaterialFlag_DiaElectric), ENUM_VALUE(MaterialFlag_Shadows));
	}

	REFLECT(BDRFMaterial)
	{
		BEGIN_REFLECT(BDRFMaterial)
		REFLECT_CONSTRUCTOR()
		REFLECT_PROPERTY("Albedo", Albedo)
		REFLECT_PROPERTY("Metallic", Metallic)(META_DATA(EPropertyMetaData_Max, 1.0f))(
			META_DATA(EPropertyMetaData_Min, 0.0f))
			REFLECT_PROPERTY("Roughness", Roughness)(META_DATA(EPropertyMetaData_Max, 1.0f))(META_DATA(
				EPropertyMetaData_Min, 0.0f))REFLECT_PROPERTY("Emission", Emission)(META_DATA(EPropertyMetaData_HDR, true))
				REFLECT_PROPERTY("Opacity", Opacity)(META_DATA(EPropertyMetaData_Max, 1.0f))(
					META_DATA(EPropertyMetaData_Min, 0.0f))REFLECT_PROPERTY("Tiling", Tiling)
					REFLECT_PROPERTY("DepthScale", DepthScale)
						REFLECT_PROPERTY("Flags", Flags)(META_DATA(EPropertyFlags_BitFlags, true));
	}
} // namespace BHive