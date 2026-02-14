#include "gfx/RenderCommand.h"
#include "gfx/Shader.h"
#include "gfx/ShaderManager.h"
#include "gfx/Texture.h"
#include "gfx/UniformBuffer.h"
#include "renderers/Renderer.h"
#include "StandardMaterial.h"

namespace BHive
{
	StandardMaterial::StandardMaterial()
		: Material(nullptr)
	{
		AddTextureSlot("Albedo", 0);
		AddTextureSlot("Normal", 1);
		AddTextureSlot("Roughness", 2);
		AddTextureSlot("Metallic", 3);
		AddTextureSlot("Emission", 4);
		AddTextureSlot("Opacity", 5);
	}

	void StandardMaterial::Submit(Ref<Pipeline> pipeline)
	{

		Material::Submit(pipeline);

		/*shader->SetUniform<glm::vec3>("constants.Albedo", Albedo);
		shader->SetUniform<glm::vec3>("constants.Emission", Emission);
		shader->SetUniform("constants.Roughness", Roughness);
		shader->SetUniform("constants.Metalness", Metallic);
		shader->SetUniform("constants.Opacity", Opacity);
		shader->SetUniform("constants.Tiling", Tiling);
		shader->SetUniform("constants.Flags", (uint32_t)Flags);
		shader->SetUniform("constants.HasNormalMap", mTextures.at("Normal").Texture != nullptr);*/
	}

	/*Ref<Shader> StandardMaterial::GetShader() const
	{
		static Ref<Shader> shader = ShaderManager::Get().Load(ENGINE_SHADER_PATH "/BDRFMaterial.glsl");
		return shader;
	}*/

	void StandardMaterial::Save(cereal::BinaryOutputArchive &ar) const
	{
		Material::Save(ar);
		ar(Albedo, Emission, Metallic, Roughness, Opacity, DepthScale, Tiling, Flags);
	}

	void StandardMaterial::Load(cereal::BinaryInputArchive &ar)
	{
		Material::Load(ar);
		ar(Albedo, Emission, Metallic, Roughness, Opacity, DepthScale, Tiling, Flags);
	}

	bool StandardMaterial::ShouldCastShadows() const
	{
		return (Flags & CastShadows) != 0;
	}

	/*Ref<Material> StandardMaterial::Clone() const
	{
		return CreateRef<StandardMaterial>(*this);
	}*/

	REFLECT(StandardMaterial::EFlags)
	{
		BEGIN_REFLECT_ENUM(StandardMaterial::EFlags)(ENUM_VALUE(CastShadows), ENUM_VALUE(ReceiveShadows), ENUM_VALUE(DiaElectric), ENUM_VALUE(Shadows));
	}

	REFLECT(StandardMaterial)
	{
		BEGIN_REFLECT(StandardMaterial)
		REFLECT_CONSTRUCTOR()
		REFLECT_PROPERTY("Albedo", Albedo)
		REFLECT_PROPERTY("Metallic", Metallic)(META_DATA(EPropertyMetaData_Max, 1.0f))(META_DATA(EPropertyMetaData_Min, 0.0f))REFLECT_PROPERTY("Roughness", Roughness)(
			META_DATA(EPropertyMetaData_Max, 1.0f))(META_DATA(EPropertyMetaData_Min, 0.0f))REFLECT_PROPERTY("Emission", Emission)(META_DATA(EPropertyMetaData_HDR, true))
			REFLECT_PROPERTY("Opacity", Opacity)(META_DATA(EPropertyMetaData_Max, 1.0f))(META_DATA(EPropertyMetaData_Min, 0.0f))REFLECT_PROPERTY("Tiling", Tiling)
				REFLECT_PROPERTY("DepthScale", DepthScale) REFLECT_PROPERTY("Flags", Flags)(META_DATA(EPropertyFlags_BitFlags, true));

		rttr::type::register_wrapper_converter_for_base_classes<Ref<StandardMaterial>>();
	}
} // namespace BHive