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
		
		CreateDescriptorResources();
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

		UpdateDescriptorResources();
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

		ar(mTextures);
	}

	void Material::Load(cereal::BinaryInputArchive &ar)
	{

		Asset::Load(ar);

		ar(mTextures);

		if (mShader)
		{
			CreateDescriptorResources();
		}
	}

	void Material::CreateDescriptorResources()
	{
		DestroyDescriptorResources();

		if (!mShader)
			return;

		const auto &reflection_data = mShader->GetRelectionData();

		if (reflection_data.Samplers.empty())
			return;

		FDescriptorSetLayout::Builder builder;
		for (auto &[name, sampler] : reflection_data.Samplers)
		{
			builder.AddBinding(sampler.Binding, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment, 1);
			mTextures.emplace(name, TextureSlot{static_cast<uint32_t>(sampler.Binding), nullptr});
		}

		for (auto &[name, uniform_buffers] : reflection_data.UniformBuffers)
		{
			builder.AddBinding(uniform_buffers.Binding, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment, 1);
		}

		auto num_samplers = static_cast<uint32_t>(reflection_data.Samplers.size());
		auto num_uniform_buffers = static_cast<uint32_t>(reflection_data.UniformBuffers.size());
		auto max_sets = num_samplers + num_uniform_buffers;

		mDescriptorSetLayout = builder.Build();
		mDescriptorPool = FDescriptorPool::Builder()
							  .SetMaxSets(max_sets * 2)
							  .AddPoolSize(vk::DescriptorType::eCombinedImageSampler, num_samplers * VulkanCore::MAX_FRAMES_IN_FLIGHT)
							  .AddPoolSize(vk::DescriptorType::eUniformBuffer, num_uniform_buffers * VulkanCore::MAX_FRAMES_IN_FLIGHT)
							  .Build();
		FDescriptorWriter(mDescriptorSetLayout, mDescriptorPool).Build(mDescriptorSets);
	}

	void Material::DestroyDescriptorResources()
	{
		mDescriptorSets.clear();
		mDescriptorPool = nullptr;
		mDescriptorSetLayout = nullptr;
	}

	void Material::UpdateDescriptorResources()
	{
		if (!mDescriptorSetLayout || !mDescriptorPool)
			return;

		auto api = RenderCommand::GetAPI();
		for (auto &[name, slot] : mTextures)
		{
			
			Ref<Texture> texture = nullptr;
			if (mTextures.contains(name) && mTextures.at(name).Texture)
			{
				texture = mTextures.at(name).Texture;
			}
			else
			{
				texture = Renderer::GetWhiteTexture();
			}

			auto image_info = reinterpret_cast<vk::DescriptorImageInfo *>(texture->GetNativeHandle());
			FDescriptorWriter(mDescriptorSetLayout, mDescriptorPool).WriteImage(slot.Binding, *image_info).Overwrite(mDescriptorSets);
			
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

		rttr::type::register_wrapper_converter_for_base_classes<Ref<Material>>();
	}

} // namespace BHive