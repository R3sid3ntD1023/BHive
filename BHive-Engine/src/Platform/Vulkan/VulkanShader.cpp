#include "VulkanShader.h"
#include "VulkanConversions.h"
#include "gfx/renderers/Renderer.h"

namespace BHive
{
	namespace utils
	{
		bool IsImage(vk::DescriptorType type)
		{
			return type == vk::DescriptorType::eCombinedImageSampler || type == vk::DescriptorType::eStorageImage;
		}
	} // namespace utils

	VulkanShader::VulkanShader(const Ref<ShaderAsset> &asset)
		: ShaderProgram(asset),
		  mDevice(VulkanBackend::GetLogicalDevice())
	{
		CreateDescriptorResources(*asset);
		CreateModules(*asset);
		CreatePipelineLayout();
	}

	void VulkanShader::BindGlobal(uint32_t set, uint32_t binding, const Ref<BufferBase> &buffer)
	{
		if (auto group = GetBindingGroup(set))
		{
			group->SetBuffer(binding, buffer);
		}
	}

	void VulkanShader::BindGlobal(uint32_t set, uint32_t binding, const Ref<Texture> &texture)
	{
		if (auto group = GetBindingGroup(set))
		{
			group->SetTexture(binding, texture);
		}
	}

	void VulkanShader::Bind(vk::CommandBuffer cmd, uint32_t frame)
	{

		for (auto &[stage, shader] : mShaderEXTs)
			cmd.bindShadersEXT(stage, {shader});

		for (auto &[setIndex, group] : mBindGroups)
		{
			auto set = group->Update(frame);
			cmd.bindDescriptorSets(mBindPoint, mPipelineLayout, setIndex, {set}, {});
		}
	}

	void VulkanShader::BindPushConstants(vk::CommandBuffer cmd, vk::ShaderStageFlags stage, const void *data, uint32_t size, uint32_t offset)
	{
		vk::PushConstantsInfo info(mPipelineLayout, stage, offset, size, data);
		cmd.pushConstants2(info);
	}

	vk::DescriptorSetLayout VulkanShader::GetDescriptorSetLayout(uint32_t set) const
	{
		if (mDescriptorSetLayouts.contains(set))
		{
			return mDescriptorSetLayouts.at(set);
		}

		return mEmptyDescriptorSet;
	}

	bool VulkanShader::HasSet(uint32_t setIndex) const
	{
		return mDescriptorSetLayouts.contains(setIndex);
	}

	FPipelineLayoutInfo VulkanShader::GetPipelineLayoutInfo() const
	{
		FPipelineLayoutInfo info;
		info.PushConstants = mPushConstantRanges;

		uint32_t maxSet = 0;
		for (auto &[set, _] : mDescriptorSetLayouts)
			maxSet = std::max(maxSet, set);

		info.SetLayouts.resize(maxSet + 1, mEmptyDescriptorSet);

		for (auto &[setIndex, layout] : mDescriptorSetLayouts)
		{
			info.SetLayouts[setIndex] = layout;
			info.UsedSets.push_back(setIndex);
		}

		return info;
	}

	void VulkanShader::CreateModules(const ShaderAsset &asset)
	{
		std::vector<vk::ShaderCreateInfoEXT> infos;
		infos.reserve(asset.Stages.size());

		auto layoutInfo = GetPipelineLayoutInfo();
		const auto &layouts = layoutInfo.SetLayouts;
		const auto &pushConstants = layoutInfo.PushConstants;

		std::vector<EShaderStage> stages;
		for (auto &[stage, _] : asset.Stages)
		{
			stages.push_back(stage);
		}

		uint32_t current = 0;
		for (auto &[stage, data] : asset.Stages)
		{
			auto stageBit = ToSingleVkStage(stage);

			vk::ShaderStageFlags nextStage{};

			if ((++current) < stages.size())
				nextStage = ToSingleVkStage(stages[current]);

			vk::ShaderCreateInfoEXT info(
				{}, stageBit, nextStage, vk::ShaderCodeTypeEXT::eSpirv, data.Spirv.size() * sizeof(uint32_t), data.Spirv.data(), "main", layouts.size(), layouts.data(), pushConstants.size(),
				pushConstants.data());

			mShaderEXTs.emplace(stageBit, mDevice.createShaderEXT(info));
		}

		mBindPoint = asset.Stages.contains(EShaderStage::Compute) ? vk::PipelineBindPoint::eCompute : vk::PipelineBindPoint::eGraphics;
	}

	void VulkanShader::CreatePipelineLayout()
	{
		auto layoutInfo = GetPipelineLayoutInfo();
		const auto &layouts = layoutInfo.SetLayouts;
		const auto &pushConstants = layoutInfo.PushConstants;

		vk::PipelineLayoutCreateInfo info{};
		info.setSetLayouts(layouts);
		info.setPushConstantRanges(pushConstants);

		mPipelineLayout = mDevice.createPipelineLayout(info);
	}

	void VulkanShader::CreateDescriptorResources(const ShaderAsset &asset)
	{
		const auto &merged = asset.MergedReflection;

		mMaxSet = 0;
		mDescriptorSetLayouts.clear();

		for (auto &[set, refl] : merged.Sets)
		{
			mMaxSet = std::max(mMaxSet, set);

			auto &refl = merged.Sets.at(set);

			std::vector<vk::DescriptorSetLayoutBinding> bindings;

			for (auto &[name, sampler] : refl.Samplers)
			{
				auto vk_stage = ToVkShaderStageBit(sampler.Stages);
				bindings.emplace_back(sampler.Binding, ToVkType(sampler.Type), sampler.ArraySize, vk_stage);
			}

			for (auto &[name, ubo] : refl.UniformBuffers)
			{
				auto vk_stage = ToVkShaderStageBit(ubo.Stages);
				auto type = vk::DescriptorType::eUniformBuffer;

				bindings.emplace_back(ubo.Binding, type, 1, vk_stage);
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
				auto &flags = binding_flags[i];

				if (utils::IsImage(b.descriptorType))
				{
					flags = vk::DescriptorBindingFlagBits::ePartiallyBound | vk::DescriptorBindingFlagBits::eUpdateUnusedWhilePending;
				}
			}

			vk::DescriptorSetLayoutBindingFlagsCreateInfo flags(binding_flags);
			vk::DescriptorSetLayoutCreateInfo layout_info({}, bindings, bindings.empty() ? nullptr : &flags);
			mDescriptorSetLayouts.emplace(set, mDevice.createDescriptorSetLayout(layout_info));

			auto group = CreateRef<VulkanBindingGroup>(this, set);
			mBindGroups.emplace(set, group);
		}

		vk::DescriptorSetLayoutCreateInfo empty{};
		mEmptyDescriptorSet = mDevice.createDescriptorSetLayout(empty);

		for (auto &pc : merged.PushConstants)
		{
			mPushConstantRanges.emplace_back(ToVkShaderStageBit(pc.Stages), pc.Offset, (uint32_t)pc.Size);
		}

		// LOG_INFO("Push constants found: {} - {}", asset.Name, merged.PushConstants.size());
	}

	VulkanBindingGroup *VulkanShader::GetBindingGroup(uint32_t set) const
	{
		if (mBindGroups.contains(set))
			return mBindGroups.at(set).get();

		return nullptr;
	}

} // namespace BHive