#include "gfx/Shader.h"
#include "gfx/ShaderManager.h"
#include "LambertMaterial.h"

namespace BHive
{

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

	void LambertMaterial::Submit(Pipeline* pipeline)
	{
		Material::Submit(pipeline);

		mBackendMaterial->Set("DiffuseColor", DiffuseColor);
		mBackendMaterial->Set("EmissiveColor", EmissionColor);
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