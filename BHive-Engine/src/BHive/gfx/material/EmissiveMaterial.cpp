#include "EmissiveMaterial.h"

namespace BHive
{
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

	void EmissiveMaterial ::Submit(Pipeline* pipeline)
	{
		Material::Submit(pipeline);

		mBackendMaterial->Set("EmissiveColor", EmissionColor);
	}

	/*Ref<Shader> EmissiveMaterial::GetShader() const
	{
		static Ref<Shader> shader = ShaderManager::Get().Load(ENGINE_SHADER_PATH "/Emissive.glsl");
		return shader;
	}*/

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