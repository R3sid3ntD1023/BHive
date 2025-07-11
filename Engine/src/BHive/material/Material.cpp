#include "gfx/RenderCommand.h"
#include "gfx/Shader.h"
#include "gfx/ShaderManager.h"
#include "gfx/Texture.h"
#include "Material.h"
#include "renderers/Renderer.h"

namespace BHive
{

	Material::Material(const Ref<Shader> &shader)
		: mShader(shader)
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
		ar(mTextureSlots);

		// Save all texture handles
		for (auto &[name, texture] : mTextures)
		{
			ar(TAssetHandle(texture));
		}
	}

	void Material::Load(cereal::BinaryInputArchive &ar)
	{

		Asset::Load(ar);
		ar(mTextureSlots);

		for (auto &slot : mTextureSlots)
		{
			ar(TAssetHandle(mTextures[slot.first]));
		}
	}

	REFLECT(Material)
	{
		BEGIN_REFLECT(Material)
		REFLECT_PROPERTY("Textures", mTextures);
	}

} // namespace BHive