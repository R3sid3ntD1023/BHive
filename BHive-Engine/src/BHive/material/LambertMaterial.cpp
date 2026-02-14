#include "gfx/Shader.h"
#include "gfx/ShaderManager.h"
#include "LambertMaterial.h"

namespace BHive
{
	LambertMaterial::LambertMaterial()
		: Material(nullptr)
	{
		AddTextureSlot("Texture", 0);
	}

	void LambertMaterial::Save(cereal::BinaryOutputArchive &ar) const
	{
		Material::Save(ar);
		ar(MAKE_NVP("Color", DiffuseColor), MAKE_NVP("Emission", EmissionColor));
	}

	void LambertMaterial::Load(cereal::BinaryInputArchive &ar)
	{
		Material::Load(ar);
		ar(MAKE_NVP("Color", DiffuseColor), MAKE_NVP("Emission", EmissionColor));
	}

	void LambertMaterial::Submit(Ref<Pipeline>  pipeline)
	{
		Material::Submit(pipeline);

	/*	shader->SetUniform<glm::vec4>("constants.DiffuseColor", DiffuseColor);
		shader->SetUniform<glm::vec3>("constants.EmissiveColor", EmissionColor);*/
	}

	/*Ref<Shader> LambertMaterial::GetShader() const
	{
		static Ref<Shader> shader = ShaderManager::Get().Load(ENGINE_SHADER_PATH "/Lambert.glsl");
		return shader;
	}*/

	/*Ref<Material> LambertMaterial::Clone() const
	{
		return CreateRef<LambertMaterial>(*this);
	}*/

	REFLECT(LambertMaterial)
	{
		BEGIN_REFLECT(LambertMaterial)
		REFLECT_PROPERTY(DiffuseColor)
		REFLECT_PROPERTY(EmissionColor)
		REFLECT_CONSTRUCTOR();
	}
} // namespace BHive