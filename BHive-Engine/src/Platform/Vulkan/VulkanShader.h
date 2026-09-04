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
		using PushConstantRanges = std::vector<vk::PushConstantRange>;
		using SetHashes = std::map<uint64_t, uint64_t>;

	public:
		VulkanShader(const ShaderAsset &asset);

		void Bind(vk::CommandBuffer cmd);

		void BindGroup(vk::CommandBuffer cmd, uint32_t frame, IBindingGroup *group);

		void BindPushConstants(vk::CommandBuffer cmd, vk::ShaderStageFlags stage, const void *data, uint32_t size, uint32_t offset);

		const uint32_t GetSetCount() const { return (uint32_t)mDescriptorSetLayouts.size(); }

		vk::DescriptorSetLayout GetDescriptorSetLayout(uint32_t set) const;

		bool HasSet(uint32_t setIndex) const;

		FPipelineLayoutInfo GetPipelineLayoutInfo() const;

	private:
		void CreateModules(const ShaderAsset &asset);

		void CreateDescriptorResources(const ShaderAsset &asset);

		void CreateSetHashes(const ShaderAsset &asset);

		void CreatePipelineLayout();

	private:
		vk::raii::Device &mDevice;

		vk::raii::PipelineLayout mPipelineLayout = VK_NULL_HANDLE;

		vk::PipelineBindPoint mBindPoint = vk::PipelineBindPoint::eGraphics;

		std::unordered_map<vk::ShaderStageFlagBits, vk::raii::ShaderEXT> mShaderEXTs;

		std::map<uint32_t, vk::raii::DescriptorSetLayout> mDescriptorSetLayouts;

		// std::unordered_map<uint32_t, Ref<VulkanBindingGroup>> mBindGroups;

		vk::raii::DescriptorSetLayout mEmptyDescriptorSet = VK_NULL_HANDLE;

		PushConstantRanges mPushConstantRanges;

		uint32_t mMaxSet = 0;
	};
} // namespace BHive