#pragma once

#include "core/Core.h"
#include "VulkanBackend.h"
#include "gfx/shader/ShaderProgram.h"
#include "VulkanBindingGroup.h"

namespace BHive
{

	struct FPipelineLayoutInfo
	{
		std::vector<vk::DescriptorSetLayout> SetLayouts;
		std::vector<vk::PushConstantRange> PushConstants;
		std::vector<uint32_t> UsedSets;
	};

	class BHIVE_API VulkanShader : public ShaderProgram
	{
	public:
		using PushConstantRanges = std::vector<vk::PushConstantRange>;
		using SetHashes = std::map<uint64_t, uint64_t>;

	public:
		VulkanShader(const Ref<ShaderAsset> &asset);

		const uint32_t GetSetCount() const { return (uint32_t)mDescriptorSetLayouts.size(); }

		vk::DescriptorSetLayout GetDescriptorSetLayout(uint32_t set) const;

		const SetHashes &GetSetHashes() const { return mSetHashes; }

		bool HasSet(uint32_t setIndex) const;

		FPipelineLayoutInfo GetPipelineLayoutInfo() const;

		VulkanBindingGroup *GetBindingGroup(uint32_t set) const;

		void Bind(vk::CommandBuffer cmd, uint32_t frame);

		void BindPushConstants(vk::CommandBuffer cmd, vk::ShaderStageFlags stage, const void *data, uint32_t size, uint32_t offset);

	private:
		void CreateModules(const ShaderAsset &asset);

		void CreateDescriptorResources(const ShaderAsset &asset);

		void CreateSetHashes(const ShaderAsset &asset);

		void CreatePipelineLayout();

		uint64_t HashSetLayout(const FShaderReflection &merged, uint32_t set);

		void BindGlobalResources();

	private:
		vk::raii::Device &mDevice;

		vk::raii::PipelineLayout mPipelineLayout = VK_NULL_HANDLE;

		vk::PipelineBindPoint mBindPoint = vk::PipelineBindPoint::eGraphics;

		std::unordered_map<vk::ShaderStageFlagBits, vk::raii::ShaderEXT> mShaderEXTs;

		std::map<uint32_t, vk::raii::DescriptorSetLayout> mDescriptorSetLayouts;

		std::map<uint32_t, Scope<VulkanBindingGroup>> mBindGroups;

		vk::raii::DescriptorSetLayout mEmptyDescriptorSet = VK_NULL_HANDLE;

		SetHashes mSetHashes;

		PushConstantRanges mPushConstantRanges;

		uint32_t mMaxSet = 0;
	};
} // namespace BHive