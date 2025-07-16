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
			mTextures[name].Texture = texture;
		}
	}

	void Material::Submit(const Ref<Shader> &shader)
	{
		for (auto &[name, slot] : mTextures)
		{
			if (slot.Texture)
			{
				slot.Texture->Bind(slot.Binding);
			}
			else
			{
				Renderer::GetWhiteTexture()->Bind(slot.Binding);
				continue;
			}
		}
	}

	void Material::AddTextureSlot(const std::string &name, uint32_t binding)
	{
		if (!mTextures.contains(name))
		{
			mTextures.emplace(name, TextureSlot{binding, nullptr});
		}
	}

	Ref<Shader> Material::GetShader() const
	{
		return mShader;
	}

	void Material::Save(cereal::BinaryOutputArchive &ar) const
	{
		Asset::Save(ar);

		// Save all texture handles
		for (auto &[name, slot] : mTextures)
		{
			ar(TAssetHandle(slot.Texture));
		}
	}

	void Material::Load(cereal::BinaryInputArchive &ar)
	{

		Asset::Load(ar);

		for (auto &[name, slot] : mTextures)
		{
			ar(TAssetHandle(slot.Texture));
		}
	}

	REFLECT(Material::TextureSlot)
	{
		BEGIN_REFLECT(Material::TextureSlot)
		REFLECT_PROPERTY("Texture", Texture);
	}

	REFLECT(Material)
	{
		BEGIN_REFLECT(Material)
		REFLECT_PROPERTY("Textures", mTextures);
	}

} // namespace BHive