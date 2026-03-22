#pragma once

#include "VulkanCore.h"
#include "gfx/Pipeline.h"

namespace BHive
{
	class VulkanShader;
	class ShaderProgram;
	class ISetManager;

	class BHIVE_API VulkanPipeline : public Pipeline
	{
	public:
		VulkanPipeline();

		~VulkanPipeline();

		virtual void Init(const GraphicsPipelineState& state) override;

		virtual void Init(const ComputePipelineState &state) override;

		virtual void Bind() override;

		virtual void UnBind() override {};

		Ref<ShaderProgram> GetShaderProgram() const override;

		const VulkanShader& GetVulkanShader() const { return *mShader.get(); }

		const vk::raii::PipelineLayout &GetLayout() const { return mPipelineLayout; }

		vk::DescriptorSetLayout GetSetLayout(uint32_t set) const;

		void SetObjectSetManager(ISetManager *manager);

		ISetManager *GetBatchSetManager() const override { return mBatchSetManager.get(); }

	private:
		vk::raii::Device &mDevice;

		std::vector<vk::raii::DescriptorSetLayout> mOwnedEmptyLayouts;

		vk::raii::PipelineLayout mPipelineLayout = VK_NULL_HANDLE;

		vk::raii::Pipeline mPipeline = VK_NULL_HANDLE;

		Ref<ShaderProgram> mProgram;

		Scope<VulkanShader> mShader;

		ISetManager *mObjectSetManager = nullptr;

		Ref<ISetManager> mBatchSetManager;
	};
} // namespace BHive