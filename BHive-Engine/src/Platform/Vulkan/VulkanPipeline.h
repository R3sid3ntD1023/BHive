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

		virtual void Init(const PipelineState& state) override;

		virtual void Bind() override;

		virtual void UnBind() override {};

		Ref<ShaderProgram> GetShaderProgram() const override;

		const VulkanShader& GetVulkanShader() const { return *mShader.get(); }

		const vk::raii::PipelineLayout &GetLayout() const { return mPipelineLayout; }

		vk::DescriptorSetLayout GetSetLayout(uint32_t set) const;

		void SetMaterialSetManager(ISetManager *manager);

		void SetObjectSetManager(ISetManager *manager);

		void SetBatchSetManager(ISetManager *manager);

	private:
		vk::raii::Device &mDevice;

		std::vector<vk::raii::DescriptorSetLayout> mOwnedEmptyLayouts;

		vk::raii::PipelineLayout mPipelineLayout = VK_NULL_HANDLE;

		vk::raii::Pipeline mPipeline = VK_NULL_HANDLE;

		Ref<ShaderProgram> mProgram;

		Scope<VulkanShader> mShader;

		ISetManager *mMaterialSetManager = nullptr;

		ISetManager *mObjectSetManager = nullptr;

		ISetManager *mBatchSetManager = nullptr;
	};
} // namespace BHive