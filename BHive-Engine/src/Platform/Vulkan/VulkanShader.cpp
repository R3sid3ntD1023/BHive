#include "VulkanShader.h"
#include "VulkanConverters.h"



namespace BHive
{
	namespace utils
	{
		bool IsImage(vk::DescriptorType type)
		{
			return type == vk::DescriptorType::eCombinedImageSampler || type == vk::DescriptorType::eStorageImage;
		}
	}

	VulkanShader::VulkanShader()
		: mDevice(VulkanBackend::GetLogicalDevice())
	{
		
	}

	void VulkanShader::Init(const Ref<ShaderAsset> &asset)
	{
		ASSERT(asset);
		CreateModules(*asset);
		CreateDescriptorResources(*asset);

		const auto &merged = asset->MergedReflection;
		for (const auto &[setIndex, layout] : mDescriptorSetLayouts)
		{
			uint64_t hash = HashSetLayout(merged, setIndex);
			mSetHashes[setIndex] = hash;
		}
	}

	vk::DescriptorSetLayout VulkanShader::GetDescriptorSetLayout(uint32_t set) const
	{
		if (mDescriptorSetLayouts.contains(set))
		{
			return mDescriptorSetLayouts.at(set);
		}
		
		return VK_NULL_HANDLE;
	}

	bool VulkanShader::HasSet(uint32_t setIndex) const
	{
		return mDescriptorSetLayouts.contains(setIndex);
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

		mMaxSet = 0;
		mDescriptorSetLayouts.clear();

		for (auto &[set, _] : merged.Sets)
			mMaxSet = std::max(mMaxSet, set);

		for (uint32_t set = 0; set <= mMaxSet; set++)
		{
			
			if (!merged.Sets.contains(set))
			{
				vk::DescriptorSetLayoutCreateInfo empty_info{};
				mDescriptorSetLayouts.emplace(set, mDevice.createDescriptorSetLayout(empty_info));
				continue;
			}

			auto &refl = merged.Sets.at(set);

			std::vector<vk::DescriptorSetLayoutBinding> bindings;

			for (auto &[name, sampler] : refl.Samplers)
			{
				auto vk_stage = ToVkShaderStageBit(sampler.Stages);
				bindings.emplace_back(sampler.Binding,ToVkType(sampler.Type), sampler.ArraySize, vk_stage);
			}

			for (auto &[name, ubo] : refl.UniformBuffers)
			{
				auto vk_stage = ToVkShaderStageBit(ubo.Stages);
				bindings.emplace_back(ubo.Binding, vk::DescriptorType::eUniformBuffer, 1, vk_stage);
			}

			for (auto &[name, sbo] : refl.StorageBuffers)
			{
				auto vk_stage = ToVkShaderStageBit(sbo.Stages);
				bindings.emplace_back(sbo.Binding, vk::DescriptorType::eStorageBuffer, 1, vk_stage);
			}

			std::vector<vk::DescriptorBindingFlags> binding_flags(bindings.size(), {});

			for (size_t i = 0; i < bindings.size(); i++)
			{
				auto &b = bindings[i];

				if (utils::IsImage(b.descriptorType))
				{
					binding_flags[i] = vk::DescriptorBindingFlagBits::ePartiallyBound | vk::DescriptorBindingFlagBits::eUpdateUnusedWhilePending | vk::DescriptorBindingFlagBits::eUpdateAfterBind;
				}
			}

			vk::DescriptorSetLayoutBindingFlagsCreateInfo flags(binding_flags);
			vk::DescriptorSetLayoutCreateInfo layout_info(vk::DescriptorSetLayoutCreateFlagBits::eUpdateAfterBindPool, bindings, bindings.empty() ? nullptr : &flags);
			mDescriptorSetLayouts.emplace(set, mDevice.createDescriptorSetLayout(layout_info));
		}

		for (auto& pc : merged.PushConstants)
		{
			mPushConstantRanges.emplace_back(ToVkShaderStageBit(pc.Stages), pc.Offset, pc.Size);
		}

		//LOG_INFO("Push constants found: {} - {}", asset.Name, merged.PushConstants.size());
	}

	uint64_t VulkanShader::HashSetLayout(const FShaderReflection &merged, uint32_t set)
	{
		uint64_t h = 146527; // random seed

		auto hash_combine = [&](uint64_t v)
		{
			h ^= v += 0x9e3779b9 + (h << 6) + (h >> 2);
		};

		if (!merged.Sets.contains(set))
			return h;

		const auto &target_set = merged.Sets.at(set);

		for (auto &[name, ub] : target_set.UniformBuffers)
		{
			hash_combine(std::hash<std::string>{}(name));
			hash_combine(ub.Binding);
			hash_combine((uint64_t)vk::DescriptorType::eUniformBuffer);
			hash_combine(ub.Size);
			hash_combine(static_cast<uint64_t>(ub.Stages));
		}

		for (auto &[name, sb] : target_set.StorageBuffers)
		{
			hash_combine(std::hash<std::string>{}(name));
			hash_combine(sb.Binding);
			hash_combine((uint64_t)vk::DescriptorType::eStorageBuffer);
			hash_combine(sb.Size);
			hash_combine(static_cast<uint64_t>(sb.Stages));
		}

		for (auto &[name, sampler] : target_set.Samplers)
		{
			hash_combine(std::hash<std::string>{}(name));
			hash_combine(sampler.Binding);
			hash_combine((uint64_t)vk::DescriptorType::eCombinedImageSampler);
			hash_combine(sampler.ArraySize);
			hash_combine(static_cast<uint64_t>(sampler.Stages));
		}

		return h;
	}
} // namespace BHive