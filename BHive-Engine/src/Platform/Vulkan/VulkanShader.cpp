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

	VulkanShader::VulkanShader(const ShaderAsset &asset)
		: Shader(asset),
		  mDevice(VulkanBackend::GetLogicalDevice())
	{
		mPipelineLayoutInfo = GetPipelineLayoutInfo();

		CreateModules(asset);
		CreatePipelineLayout();
	}

	void VulkanShader::Bind(vk::CommandBuffer cmd)
	{
		for (auto &[stage, shader] : mShaderEXTs)
			cmd.bindShadersEXT(stage, {shader});
	}

	void VulkanShader::BindGroup(vk::CommandBuffer cmd, uint32_t frame, VulkanBindingGroup *group)
	{
		auto set = group->Update(frame);
		cmd.bindDescriptorSets(mBindPoint, mPipelineLayout, group->GetSetIndex(), {set}, {});
	}

	void VulkanShader::BindPushConstants(vk::CommandBuffer cmd, vk::ShaderStageFlags stage, const void *data, uint32_t size, uint32_t offset)
	{
		vk::PushConstantsInfo info(mPipelineLayout, stage, offset, size, data);
		cmd.pushConstants2(info);
	}

	FPipelineLayoutInfo VulkanShader::GetPipelineLayoutInfo() const
	{
		const auto &shaderTemplate = GetTemplate();
		auto &layoutCache = VulkanBackend::GetLayoutCache();

		FPipelineLayoutInfo info{};
		uint32_t maxSet = shaderTemplate.MaxSet;
		info.SetLayouts.resize(maxSet + 1, layoutCache.GetEmptyLayout());

		for (auto &setTemplate : shaderTemplate.Sets)
		{
			info.SetLayouts[setTemplate.SetIndex] = layoutCache.GetOrCreate(setTemplate);
		}

		for (auto &pc : shaderTemplate.PushConstants)
		{
			info.PushConstants.emplace_back(ToVkShaderStageBit(pc.Stages), pc.Offset, (uint32_t)pc.Size);
		}

		return info;
	}

	void VulkanShader::CreateModules(const ShaderAsset &asset)
	{
		std::vector<vk::ShaderCreateInfoEXT> infos;
		infos.reserve(asset.Stages.size());

		const auto &layouts = mPipelineLayoutInfo.SetLayouts;
		const auto &pushConstants = mPipelineLayoutInfo.PushConstants;

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
		const auto &layouts = mPipelineLayoutInfo.SetLayouts;
		const auto &pushConstants = mPipelineLayoutInfo.PushConstants;

		vk::PipelineLayoutCreateInfo info{};
		info.setSetLayouts(layouts);
		info.setPushConstantRanges(pushConstants);

		mPipelineLayout = mDevice.createPipelineLayout(info);
	}

} // namespace BHive