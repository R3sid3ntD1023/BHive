#include "gfx/Shader.h"
#include "gfx/Texture.h"
#include "Material.h"

namespace BHive
{

	Material::Material(const Ref<Shader> &shader)
		: mShader(shader)
	{
		ASSERT(shader)

		UpdateTextureSlots();

		CreateBackendMaterial();
	}

	void Material::SetTexture(const char *name, const Ref<Texture> &texture)
	{
		if (mTextures.contains(name))
		{
			auto &slot = mTextures[name];
			slot.Texture = texture;
			mBackendMaterial->BindTexture(slot.Binding, texture);
		}
	}

	void Material::Submit(const Ref<Shader> &shader)
	{
		auto shader_instance = shader ? shader : mShader;

		shader_instance->Bind(); // binds shaders pipeline 

		mBackendMaterial->Bind(mShader); //update descriptor sets
	}

	void Material::AddTextureSlot(const std::string &name, uint32_t binding)
	{
		if (!mTextures.contains(name))
		{
			mTextures.emplace(name, TextureSlot{binding, nullptr});
		}
	}

	void Material::UpdateTextureSlots()
	{
		auto &refl = mShader->GetRelectionData();
		for (auto &[name, info] : refl.Samplers)
		{
			mTextures.emplace(name, TextureSlot{(uint32_t)info.Binding, nullptr});
		}
	}

	void Material::CreateBackendMaterial()
	{
		mBackendMaterial = IMaterialBackendInterface::Create();
		mBackendMaterial->Init(mShader);
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

	REFLECT(Material::TextureSlot)
	{
		BEGIN_REFLECT(Material::TextureSlot)
		REFLECT_PROPERTY("Texture", Texture);
	}

	REFLECT(Material)
	{
		BEGIN_REFLECT(Material)
		REFLECT_PROPERTY("Textures", mTextures);

		rttr::type::register_wrapper_converter_for_base_classes<Ref<Material>>();
	}

} // namespace BHive