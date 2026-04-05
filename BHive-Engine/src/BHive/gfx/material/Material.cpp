#include "gfx/Shader.h"
#include "gfx/Texture.h"
#include "Material.h"
#include "gfx/renderers/Renderer.h"
#include "gfx/Pipeline.h"

namespace BHive
{

	Material::Material(Ref<Pipeline> pipeline)
		: mPipeline(pipeline)
	{
		ASSERT(pipeline)

	
		CreateBackendMaterial();

		UpdateTextureSlots();

	}

	void Material::SetTexture(const char *name, const Ref<Texture> &texture, uint32_t mip)
	{
		if (mTextures.contains(name))
		{
			mTextures[name].Texture = texture;
			mTextures[name].MipLevel = mip;
			return;
		}

		LOG_ERROR("Texture slot with name : {} doesnt exist!", name);
	}

	void Material::Submit(Ref<Pipeline> pipeline)
	{
		auto current_pipeline = pipeline ? pipeline : mPipeline;
		current_pipeline->Bind(); // binds shaders pipeline 

		for (auto& [name, slot] : mTextures)
		{
			auto tex = slot.Texture ? slot.Texture  : Renderer::GetWhiteTexture();
			mBackendMaterial->BindTexture(name, tex, slot.MipLevel);
		}

		mBackendMaterial->Bind(mPipeline); //update descriptor sets
	}

	void Material::UpdateTextureSlots()
	{
		const auto &set = mBackendMaterial->GetTargetSet();
		for (auto &[name, info] : set.Samplers)
		{
			mTextures.emplace(name, TextureSlot{nullptr});
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

	REFLECT(TextureSlot)
	{
		BEGIN_REFLECT(TextureSlot)
		REFLECT_PROPERTY("Texture", Texture);
	}

	REFLECT(Material)
	{
		BEGIN_REFLECT(Material)
		REFLECT_PROPERTY("Textures", mTextures);

		rttr::type::register_wrapper_converter_for_base_classes<Ref<Material>>();
	}

} // namespace BHive