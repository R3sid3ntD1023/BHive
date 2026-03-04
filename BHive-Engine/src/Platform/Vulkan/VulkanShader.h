#pragma once

#include "core/Core.h"
#include "gfx/shader/ShaderAsset.h"
#include "VulkanBackend.h"

namespace BHive
{
	class Pipeline;
	class FDescriptorSetLayout;
	class FDescriptorPool;

	using ShaderModules = std::unordered_map<EShaderStage, vk::raii::ShaderModule> ;
	using PushConstantRanges = std::vector<vk::PushConstantRange>;
	using SetHashes = std::map<uint64_t, uint64_t>;

	class BHIVE_API VulkanShader 
	{
	public:
		VulkanShader();

		VulkanShader(const VulkanShader &) = delete;
		VulkanShader &operator=(const VulkanShader &) = delete; 
		VulkanShader(VulkanShader&&) noexcept = default; 
		VulkanShader& operator=(VulkanShader&&) noexcept = default;

		void Init(const Ref<ShaderAsset> &asset);

		const std::map<uint32_t, vk::raii::DescriptorSetLayout> &GetLayouts() const { return mDescriptorSetLayouts; }

		const uint32_t GetSetCount() const { return (uint32_t)mDescriptorSetLayouts.size(); }

		vk::DescriptorSetLayout GetDescriptorSetLayout(uint32_t set) const;

		const ShaderModules &GetModules() const { return mShaderModules; }

		const PushConstantRanges &GetPushConstantRanges() const { return mPushConstantRanges; }

		const SetHashes& GetSetHashes() const { return mSetHashes; }

		uint32_t GetMaxSet() const { return mMaxSet; }

	private:
		void CreateModules(const ShaderAsset& asset);

		void CreateDescriptorResources(const ShaderAsset& asset);

		uint64_t HashSetLayout(const FShaderReflection& merged, uint32_t set);

	private:
		vk::raii::Device &mDevice;

		ShaderModules mShaderModules;

		std::map<uint32_t, vk::raii::DescriptorSetLayout> mDescriptorSetLayouts;

		SetHashes mSetHashes;

		PushConstantRanges mPushConstantRanges;

		uint32_t mMaxSet = 0;
	};
} // namespace BHive