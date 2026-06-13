#pragma once

#include "VulkanCore.h"
#include "gfx/Pipeline.h"
#include "VulkanShader.h"
#include "VulkanBindingGroup.h"

namespace BHive
{
	class ShaderProgram;
	class VulkanBindingGroup;

	class BHIVE_API VulkanPipeline : public Pipeline
	{
	public:
		VulkanPipeline();

		virtual void Init(const PipelineState& state) override;

		virtual void Bind() override;

		virtual void BindImmediate(vk::CommandBuffer cmd);

		virtual void UnBind() override {};

		void UpdateSets(uint32_t frame);

		IBindingGroup *GetOrCreateBindingGroup(uint32_t groupIndex) override;

		Ref<ShaderProgram> GetShaderProgram() const override;

		const VulkanShader& GetVulkanShader() const { return *mShader.get(); }

		const vk::raii::PipelineLayout &GetLayout() const { return mPipelineLayout; }

		vk::DescriptorSetLayout GetSetLayout(uint32_t set) const;

		vk::PipelineBindPoint GetBindPoint() const { return mBindPoint; }

		const vk::raii::Pipeline &GetVkPipeline() const { return mPipeline; }

		bool HasSet(uint32_t setIndex) const;

	private:
		void BindGlobalResources();

		void CreateGraphicsPipeline(const GraphicsPipelineState &state);

		void CreateComputePipeline(const ComputePipelineState &state);

	private:
		vk::raii::Device &mDevice;

		std::vector<vk::raii::DescriptorSetLayout> mOwnedEmptyLayouts;

		vk::raii::PipelineLayout mPipelineLayout = VK_NULL_HANDLE;

		vk::raii::Pipeline mPipeline = VK_NULL_HANDLE;

		vk::PipelineBindPoint mBindPoint = vk::PipelineBindPoint::eGraphics;

		Ref<ShaderProgram> mProgram;

		Scope<VulkanShader> mShader;

		std::unordered_map<uint32_t, Scope<VulkanBindingGroup>> mSetManagers;
	};
} // namespace BHive