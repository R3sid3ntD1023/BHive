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
	using DescriptorSetLayouts = std::vector<vk::raii::DescriptorSetLayout>;

	class BHIVE_API VulkanShader 
	{
	public:
		VulkanShader();

		VulkanShader(const VulkanShader &) = delete;
		VulkanShader &operator=(const VulkanShader &) = delete; 
		VulkanShader(VulkanShader&&) noexcept = default; 
		VulkanShader& operator=(VulkanShader&&) noexcept = default;

		void Init(const Ref<ShaderAsset> &asset);

		const DescriptorSetLayouts &GetDescriptorSetLayouts() const { return mDescriptorSetLayouts; }

		const ShaderModules &GetModules() const { return mShaderModules; }

	private:
		void CreateModules(const ShaderAsset& asset);

		void CreateDescriptorResources(const ShaderAsset& asset);

	private:
		vk::raii::Device &mDevice;

		ShaderModules mShaderModules;

		DescriptorSetLayouts mDescriptorSetLayouts;
	};

	

} // namespace BHive