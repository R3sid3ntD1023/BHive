#include "VulkanLayoutCache.h"
#include "VulkanBackend.h"
#include "VulkanConversions.h"
#include "gfx/shader/ShaderTemplate.h"

namespace BHive
{

	vk::DescriptorSetLayout VulkanLayoutCache::GetOrCreate(const BindingSetTemplate &setTemplate)
	{
		auto layoutHash = setTemplate.LayoutHash;
		if (mCache.contains(layoutHash))
		{
			return mCache.at(layoutHash);
		}

		BuildDescriptorSetLayout(setTemplate);
		return mCache.at(layoutHash);
	}

	vk::DescriptorSetLayout VulkanLayoutCache::GetEmptyLayout()
	{
		if (!mCache.contains(0))
		{
			auto &device = VulkanBackend::GetLogicalDevice();
			vk::DescriptorSetLayoutCreateInfo empty{};
			mCache.emplace(0, device.createDescriptorSetLayout(empty));
		}

		return mCache.at(0);
	}

	void VulkanLayoutCache::BuildDescriptorSetLayout(const BindingSetTemplate &setTemplate)
	{
		std::vector<vk::DescriptorSetLayoutBinding> bindings;

		for (auto &binding : setTemplate.Bindings)
		{
			auto vk_stage = ToVkShaderStageBit(binding.Stages);
			bindings.emplace_back(binding.Binding, ToVkType(binding.Type), binding.ArraySize ? binding.ArraySize : 1, vk_stage);
		}

		std::vector<vk::DescriptorBindingFlags> binding_flags(bindings.size(), {});

		for (size_t i = 0; i < bindings.size(); i++)
		{
			auto &b = bindings[i];
			auto &flags = binding_flags[i];

			if (b.descriptorType == vk::DescriptorType::eCombinedImageSampler || b.descriptorType == vk::DescriptorType::eStorageImage)
			{
				flags = vk::DescriptorBindingFlagBits::ePartiallyBound | vk::DescriptorBindingFlagBits::eUpdateUnusedWhilePending;
			}
		}

		vk::DescriptorSetLayoutBindingFlagsCreateInfo flags(binding_flags);
		vk::DescriptorSetLayoutCreateInfo createInfo({}, bindings, bindings.empty() ? nullptr : &flags);

		auto &device = VulkanBackend::GetLogicalDevice();
		mCache.emplace(setTemplate.LayoutHash, device.createDescriptorSetLayout(createInfo));
	}
} // namespace BHive