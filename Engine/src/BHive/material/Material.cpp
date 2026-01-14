#include "gfx/RenderCommand.h"
#include "gfx/Shader.h"
#include "gfx/ShaderManager.h"
#include "gfx/Texture.h"
#include "Material.h"
#include "renderers/Renderer.h"
#include "renderers/buffers/GlobalBuffers.h"
#include "gfx/UniformBuffer.h"
#include "gfx/GraphicsContext.h"
#include "gfx/VulkanSwapChain.h"

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
		UpdateDescriptorResources();

		mShader->Bind();

		auto *api = RenderCommand::GetAPI();
		api->BindDescriptorSets(mShader->GetPipelineLayout(), mDescriptorSets);
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

		for (auto &[name, sampler] : reflection_data.Samplers)
		{
			mTextures.emplace(name, TextureSlot{static_cast<uint32_t>(sampler.Binding), nullptr});
		}

		for (auto &[name, data] : reflection_data.UniformBuffers)
		{
			mUniformBufferBindings.push_back(data.Binding);
		}


		auto num_samplers = static_cast<uint32_t>(reflection_data.Samplers.size());
		auto num_uniform_buffers = static_cast<uint32_t>(reflection_data.UniformBuffers.size());
		auto max_sets = num_samplers + num_uniform_buffers;
		FDescriptorPool::Builder pool_builder;
		pool_builder.SetMaxSets(max_sets * 2);

		if (num_uniform_buffers)
		{
			pool_builder.AddPoolSize(vk::DescriptorType::eUniformBuffer, num_uniform_buffers * VulkanCore::MAX_FRAMES_IN_FLIGHT);
		}

		if (num_samplers)
		{
			pool_builder.AddPoolSize(vk::DescriptorType::eCombinedImageSampler, num_samplers * VulkanCore::MAX_FRAMES_IN_FLIGHT);
		}
			
			
		mDescriptorPool = pool_builder.Build();

		auto& descriptor_set_layout = mShader->GetDescriptorSetLayout();
		FDescriptorWriter(descriptor_set_layout, mDescriptorPool).Build(mDescriptorSets);
	}

	void Material::DestroyDescriptorResources()
	{
		mDescriptorSets.clear();
		mDescriptorPool = nullptr;
	}

	void Material::UpdateDescriptorResources()
	{
		if (!mDescriptorPool)
			return;

		auto &context = GraphicsContext::Get();
		auto &swap_chain = context.GetSwapChain();
		auto current_frame = swap_chain->GetCurrentFrame();


		for (const auto& binding : mUniformBufferBindings)
		{
			auto ubo = GlobalBuffers::GetUniformBuffer(binding);
			for (const auto& set : mDescriptorSets)
				ubo->WriteDescriptor(set);
		}

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
			auto &descriptor_set_layout = mShader->GetDescriptorSetLayout();
			FDescriptorWriter(descriptor_set_layout, mDescriptorPool).WriteImage(slot.Binding, *image_info).Overwrite(mDescriptorSets);
			
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