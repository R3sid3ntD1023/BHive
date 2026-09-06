#pragma once

#include "VulkanBackend.h"
#include "VulkanBindingGroup.h"
#include "core/Core.h"
#include "gfx/shader/Shader.h"

namespace BHive
{

	struct FPipelineLayoutInfo
	{
		std::vector<vk::DescriptorSetLayout> SetLayouts;
		std::vector<vk::PushConstantRange> PushConstants;
		std::vector<uint32_t> UsedSets;
	};

	class BHIVE_API VulkanShader : public Shader
	{
	public:
		VulkanShader(const ShaderAsset &asset);

		void Bind(vk::CommandBuffer cmd);

		void BindGroup(vk::CommandBuffer cmd, uint32_t frame, VulkanBindingGroup *group);

		void BindPushConstants(vk::CommandBuffer cmd, vk::ShaderStageFlags stage, const void *data, uint32_t size, uint32_t offset);

		FPipelineLayoutInfo GetPipelineLayoutInfo() const;

	private:
		void CreateModules(const ShaderAsset &asset);

		void CreatePipelineLayout();

	private:
		vk::raii::Device &mDevice;

		vk::raii::PipelineLayout mPipelineLayout = VK_NULL_HANDLE;

		vk::PipelineBindPoint mBindPoint = vk::PipelineBindPoint::eGraphics;

		std::unordered_map<vk::ShaderStageFlagBits, vk::raii::ShaderEXT> mShaderEXTs;

		FPipelineLayoutInfo mPipelineLayoutInfo;
	};
} // namespace BHive