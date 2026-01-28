#include "EmissiveMaterial.h"
#include "gfx/Shader.h"
#include "gfx/ShaderManager.h"

namespace BHive
{
	EmissiveMaterial::EmissiveMaterial()
		: Material(GetShader())
	{
	}

	void EmissiveMaterial::Save(cereal::BinaryOutputArchive &ar) const
	{
		Material::Save(ar);
		ar(MAKE_NVP("Emission", EmissionColor));
	}

	void EmissiveMaterial ::Load(cereal::BinaryInputArchive &ar)
	{
		Material::Load(ar);
		ar(MAKE_NVP("Emission", EmissionColor));
	}

	void EmissiveMaterial ::Submit(const Ref<Shader> &shader)
	{
		Material::Submit(shader);

		shader->SetUniform<glm::vec3>("constants.EmissiveColor", EmissionColor);
	}

	Ref<Shader> EmissiveMaterial::GetShader() const
	{
		static Ref<Shader> shader = ShaderManager::Get().Load(ENGINE_SHADER_PATH "/Emissive.glsl");
		return shader;
	}

	//Ref<Material> EmissiveMaterial::Clone() const
	//{
	//	return CreateRef<EmissiveMaterial>(std::move(*this));
	//}

	REFLECT(EmissiveMaterial)
	{
		BEGIN_REFLECT(EmissiveMaterial)
		REFLECT_PROPERTY(EmissionColor)(META_DATA(EPropertyMetaData_HDR, true)) REFLECT_CONSTRUCTOR();
	}
} // namespace BHive