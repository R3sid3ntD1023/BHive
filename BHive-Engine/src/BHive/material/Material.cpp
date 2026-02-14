#include "gfx/Shader.h"
#include "gfx/Texture.h"
#include "Material.h"
#include "renderers/Renderer.h"
#include "BackendMaterial.h"
#include "gfx/Pipeline.h"
#include "gfx/ShaderManager.h"

namespace BHive
{

	Material::Material(Ref<Pipeline> pipeline)
		: mPipeline(pipeline)
	{
		ASSERT(pipeline)

		UpdateTextureSlots();

		CreateBackendMaterial();
	}

	void Material::SetTexture(const char *name, const Ref<Texture> &texture)
	{
		if (mTextures.contains(name))
		{
			mTextures[name].Texture = texture;
		}
	}

	void Material::Submit(Ref<Pipeline> pipeline)
	{
		auto current_pipeline = pipeline ? pipeline : mPipeline;
		current_pipeline->Bind(); // binds shaders pipeline 

		for (auto& [name, slot] : mTextures)
		{
			auto tex = slot.Texture ? slot.Texture  : Renderer::GetWhiteTexture();
			mBackendMaterial->BindTexture(slot.Binding, tex);
		}

		mBackendMaterial->Bind(mPipeline); //update descriptor sets
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
		auto &refl = mPipeline->GetShader()->GetRefl();
		for (auto &[name, info] : refl.Samplers)
		{
			mTextures.emplace(name, TextureSlot{(uint32_t)info.Binding, nullptr});
		}
	}

	void Material::CreateBackendMaterial()
	{
		mBackendMaterial = IMaterialBackendInterface::Create();
		mBackendMaterial->Init(mPipeline);
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