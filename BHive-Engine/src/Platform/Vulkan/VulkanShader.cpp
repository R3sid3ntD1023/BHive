#include "VulkanShader.h"
#include "VulkanConverters.h"


namespace BHive
{
	VulkanShader::VulkanShader()
		: mDevice(VulkanBackend::GetLogicalDevice())
	{
		
	}

	void VulkanShader::Init(const Ref<ShaderAsset> &asset)
	{
		ASSERT(asset);
		CreateModules(*asset);
		CreateDescriptorResources(*asset);
	}

	void VulkanShader::CreateModules(const ShaderAsset &asset)
	{
		for (auto &[stage, data] : asset.Stages)
		{
			vk::ShaderModuleCreateInfo create_info({}, data.Spirv);
			mShaderModules.emplace(stage, mDevice.createShaderModule(create_info));
		}
	}

	void VulkanShader::CreateDescriptorResources(const ShaderAsset &asset)
	{
		auto merged = FShaderReflection::Merge(asset.Reflection);

		std::unordered_map<uint32_t, std::vector<vk::DescriptorSetLayoutBinding>> bindings;

		for (auto &[name, sampler] : merged.Samplers)
		{
			auto vk_stage = Vulkan::ToVkShaderStageBit(sampler.Stages);
			bindings[sampler.Set].emplace_back(sampler.Binding, vk::DescriptorType::eCombinedImageSampler, 1, vk_stage);
		}

		for (auto &[name, ubo] : merged.UniformBuffers)
		{
			auto vk_stage = Vulkan::ToVkShaderStageBit(ubo.Stages);
			bindings[ubo.Set].emplace_back(ubo.Binding, vk::DescriptorType::eUniformBuffer, 1, vk_stage);
		}

		for (auto &[name, sbo] : merged.StorageBuffers)
		{
			auto vk_stage = Vulkan::ToVkShaderStageBit(sbo.Stages);
			bindings[sbo.Set].emplace_back(sbo.Binding, vk::DescriptorType::eStorageBuffer, 1, vk_stage);
		}

		for (auto &[setIndex, bindingsList] : bindings)
		{
			vk::DescriptorSetLayoutCreateInfo layout_info({}, bindingsList, nullptr);
			mDescriptorSetLayouts.emplace_back(mDevice.createDescriptorSetLayout(layout_info));
		}
	}




} // namespace BHive