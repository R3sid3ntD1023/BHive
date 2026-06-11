#include "gfx/Shader.h"
#include "gfx/Texture.h"
#include "Material.h"
#include "gfx/renderers/Renderer.h"
#include "gfx/Pipeline.h"

namespace BHive
{
	void Material::SetPipeline(Pipeline *pipeline)
	{
		ASSERT(pipeline)

		if (!mBackendMaterial)
			mBackendMaterial = IMaterialBackendInterface::Create();

		mPipeline = pipeline;

		mBackendMaterial->Init(pipeline);

		BuildSlotsForPipeline(pipeline);
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

		for (auto& [pipeline, slots] : mSlotsPerPipeline)
		{
			if (slots.contains(name))
			{
				switch (bindingInfo.Type)
				{
				case EResourceType::StorageImage:
					mBackendMaterial->BindTexture(name, slot.Texture, slot.MipLevel, mPipeline);
					break;
				default:
					slots[name] = slot;
				}
			}
		}
		
	}

	
	void Material::Submit(Pipeline *pipeline)
	{
		auto p = pipeline ? pipeline : mPipeline;
		p->Bind(); // binds shaders pipeline 

		auto &slots = mSlotsPerPipeline[p];
		for (auto& [name, slot] : slots)
		{
			auto res = Renderer::Get().GetGlobalResources().Find("White");
			auto tex = slot.Texture ? slot.Texture  : res->TextureRef;
			mBackendMaterial->BindTexture(name, tex, slot.MipLevel, p);
		}

		mBackendMaterial->Bind(mPipeline); //update descriptor sets
	}

	void Material::BuildSlotsForPipeline(Pipeline* pipeline)
	{
		TextureSlotMap slots;

		const auto &set = mBackendMaterial->GetTargetSet();
		for (auto &[name, info] : set.Samplers)
		{
			slots[name] = TextureSlot{nullptr};
		}

		mSlotsPerPipeline[pipeline] = std::move(slots);
	}

	void Material::Save(cereal::BinaryOutputArchive &ar) const
	{
		Asset::Save(ar);

		ar(mUserTextureSlots);
	}

	void Material::Load(cereal::BinaryInputArchive &ar)
	{

		Asset::Load(ar);

		ar(mUserTextureSlots);
	}

	REFLECT(TextureSlot)
	{
		BEGIN_REFLECT(TextureSlot)
		REFLECT_PROPERTY("Texture", Texture);
	}

	REFLECT(Material)
	{
		BEGIN_REFLECT(Material)
		REFLECT_PROPERTY("TextureSlots", mUserTextureSlots);

		rttr::type::register_wrapper_converter_for_base_classes<Ref<Material>>();
	}

} // namespace BHive