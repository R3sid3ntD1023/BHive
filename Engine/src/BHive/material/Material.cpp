#include "gfx/RenderCommand.h"
#include "gfx/Shader.h"
#include "gfx/ShaderManager.h"
#include "gfx/Texture.h"
#include "Material.h"
#include "renderers/Renderer.h"

namespace BHive
{
	Material::Material()
		: Material(GetShader())
	{

	}

	Material::Material(const Ref<Shader> &shader)
		: mShader(shader)
	{
		auto &data = shader->GetRelectionData();
		auto &buffers = data.at(ShaderType_Fragment).UniformBuffers;
	}

	void Material::Submit() const
	{

	}

	void Material::SetTexture(const char *name, const Ref<Texture> &texture)
	{
		if (mTextures.contains(name))
		{
			mTextures[name] = texture;
		}
	}

	Ref<Shader> Material::GetShader() const
	{
	return mShader;
	}

	void Material::Save(cereal::BinaryOutputArchive &ar) const
	{
		Asset::Save(ar);
		ar(mTextures);
	}

	void Material::Load(cereal::BinaryInputArchive &ar)
	{
		Asset::Load(ar);
		ar(mTextures);
	}

} // namespace BHive