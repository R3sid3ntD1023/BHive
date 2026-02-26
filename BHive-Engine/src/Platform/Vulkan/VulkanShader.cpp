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
		const auto& merged = asset.MergedReflection;

		std::map<uint32_t, std::vector<vk::DescriptorSetLayoutBinding>> bindings;

		for (auto &[name, sampler] : merged.Samplers)
		{
			auto vk_stage = ToVkShaderStageBit(sampler.Stages);
			bindings[sampler.Set].emplace_back(sampler.Binding, vk::DescriptorType::eCombinedImageSampler, sampler.ArraySize, vk_stage);
		}

		for (auto &[name, ubo] : merged.UniformBuffers)
		{
			auto vk_stage = ToVkShaderStageBit(ubo.Stages);
			bindings[ubo.Set].emplace_back(ubo.Binding, vk::DescriptorType::eUniformBuffer, 1, vk_stage);
		}

		for (auto &[name, sbo] : merged.StorageBuffers)
		{
			auto vk_stage = ToVkShaderStageBit(sbo.Stages);
			bindings[sbo.Set].emplace_back(sbo.Binding, vk::DescriptorType::eStorageBuffer, 1, vk_stage);
		}

		for (auto &[setIndex, bindingsList] : bindings)
		{
			std::vector<vk::DescriptorBindingFlags> binding_flags(bindingsList.size(), {});

			for (size_t i = 0; i < bindingsList.size(); i++)
			{
				auto &b = bindingsList[i];

				if (b.descriptorType == vk::DescriptorType::eCombinedImageSampler)
				{
					binding_flags[i] = vk::DescriptorBindingFlagBits::ePartiallyBound | vk::DescriptorBindingFlagBits::eUpdateUnusedWhilePending | vk::DescriptorBindingFlagBits::eUpdateAfterBind;
				}
			}

			vk::DescriptorSetLayoutBindingFlagsCreateInfo flags(binding_flags);
			vk::DescriptorSetLayoutCreateInfo layout_info(vk::DescriptorSetLayoutCreateFlagBits::eUpdateAfterBindPool, bindingsList, &flags);
			mDescriptorSetLayouts.emplace_back(mDevice.createDescriptorSetLayout(layout_info));
		}

		for (auto& pc : merged.PushConstants)
		{
			mPushConstantRanges.emplace_back(ToVkShaderStageBit(pc.Stages), pc.Offset, pc.Size);
		}
	}




} // namespace BHive