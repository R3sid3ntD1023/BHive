#include "PlanetMaterial.h"
#include "gfx/ShaderManager.h"
#include "gfx/Shader.h"

namespace BHive
{
	PlanetMaterial::PlanetMaterial()
		: Material(GetShader())
	{
		AddTextureSlot("Texture", 0);
	}

	void PlanetMaterial::Save(cereal::JSONOutputArchive &ar) const
	{
		Material::Save(ar);

		ar(MAKE_NVP("Color", Color), MAKE_NVP("Emission", Emission), MAKE_NVP("Flags", Flags));
	}

	void PlanetMaterial::Load(cereal::JSONInputArchive &ar)
	{
		Material::Load(ar);

		ar(MAKE_NVP("Color", Color), MAKE_NVP("Emission", Emission), MAKE_NVP("Flags", Flags));
	}

	void PlanetMaterial::Submit(const Ref<Shader> &shader)
	{
		Material::Submit(shader);

		shader->SetUniform<glm::vec3>("material.uColor", Color);
		shader->SetUniform<glm::vec3>("material.uEmission", Emission);
		shader->SetUniform("material.uFlags", (uint32_t)Flags);
	}

	Ref<Shader> PlanetMaterial::GetShader() const
	{
		static Ref<Shader> shader = ShaderManager::Get().Load(RESOURCE_PATH "/Shaders/Planet.glsl");
		return shader;
	}

	REFLECT(PlanetMaterial)
	{
		BEGIN_REFLECT(PlanetMaterial)
		REFLECT_CONSTRUCTOR();
	}
} // namespace BHive