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
		const auto &set = mBackendMaterial->GetTargetSet();
		const auto &samplers = set.Samplers;
		if (!samplers.contains(name))
		{
			LOG_ERROR("Texture slot with name : {} doesnt exist!", name);
			return;
		}

		auto slot = TextureSlot{texture, mip};
		auto &bindingInfo = samplers.at(name);
		switch (bindingInfo.Type)
		{
		case EResourceType::StorageImage:
			mImageSlots.at(name) =  slot;
			mBackendMaterial->BindTexture(name, slot.Texture, slot.MipLevel, mPipeline);
			break;
		default:
			mTextureSlots.at(name) = slot;
		}

		//LOG_INFO("SetTexture('{}') → binding {} kind {}", name, bindingInfo.Binding, (int)bindingInfo.Type);
	}

	void Material::Submit(Ref<Pipeline> pipeline)
	{
		auto current_pipeline = pipeline ? pipeline : mPipeline;
		current_pipeline->Bind(); // binds shaders pipeline 

		for (auto& [name, slot] : mTextureSlots)
		{
			auto tex = slot.Texture ? slot.Texture  : Renderer::GetWhiteTexture();
			mBackendMaterial->BindTexture(name, tex, slot.MipLevel, current_pipeline);
		}

		/*for (auto &[name, slot] : mImageSlots)
		{
			if (!slot.Texture)
				continue;

			mBackendMaterial->BindTexture(name, slot.Texture, slot.MipLevel, current_pipeline);
		}*/

		mBackendMaterial->Bind(mPipeline); //update descriptor sets
	}

	void Material::UpdateTextureSlots()
	{
		const auto &set = mBackendMaterial->GetTargetSet();
		for (auto &[name, info] : set.Samplers)
		{
			switch (info.Type)
			{
			case EResourceType::StorageImage:
				mImageSlots.emplace(name, TextureSlot{nullptr});
				break;
			default:
				mTextureSlots.emplace(name, TextureSlot{nullptr});
			};
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

		ar(mTextureSlots, mImageSlots);
	}

	void Material::Load(cereal::BinaryInputArchive &ar)
	{

		Asset::Load(ar);

		ar(mTextureSlots, mImageSlots);
	}

	REFLECT(TextureSlot)
	{
		BEGIN_REFLECT(TextureSlot)
		REFLECT_PROPERTY("Texture", Texture);
	}

	REFLECT(Material)
	{
		BEGIN_REFLECT(Material)
		REFLECT_PROPERTY("TextureSlots", mTextureSlots)
		REFLECT_PROPERTY("ImageSlots", mImageSlots);

		rttr::type::register_wrapper_converter_for_base_classes<Ref<Material>>();
	}

} // namespace BHive