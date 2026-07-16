#include "gfx/RenderCommand.h"
#include "gfx/Shader.h"
#include "gfx/ShaderManager.h"
#include "gfx/Texture.h"
#include "gfx/renderers/Renderer.h"
#include "StandardMaterial.h"

namespace BHive
{
	void StandardMaterial::Submit(Pipeline *pipeline)
	{
		auto p = pipeline ? pipeline : mPipeline;

		mBackendMaterial->Set("Albedo", Albedo);
		mBackendMaterial->Set("Emission", Emission);
		mBackendMaterial->Set("Roughness", Roughness);
		mBackendMaterial->Set("Metalness", Metallic);
		mBackendMaterial->Set("Opacity", Opacity);
		mBackendMaterial->Set("Tiling", Tiling);
		mBackendMaterial->Set("Flags", (uint32_t)Flags);
		mBackendMaterial->Set("HasNormalMap", mSlotsPerPipeline.at(p).at("NormalMap").Texture != nullptr);

		Material::Submit(pipeline);
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