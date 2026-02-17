#pragma once

#include "core/Core.h"
#include "gfx/Pipeline.h"
#include  "VulkanBackend.h"

namespace BHive
{
	class VulkanShader;
	class ShaderProgram;

	class BHIVE_API VulkanPipeline : public Pipeline
	{
	public:
		VulkanPipeline();

		~VulkanPipeline();

		virtual void Init(const PipelineState& state) override;

		virtual void Bind() override;

		virtual void UnBind() override;

		Ref<ShaderProgram> GetShaderProgram() const override;

		const vk::raii::PipelineLayout &GetLayout() const { return mPipelineLayout; }

		const std::vector<vk::DescriptorSetLayout> &GetDescriptorLayouts() const;

	private:
		vk::raii::Device &mDevice;

		vk::raii::PipelineLayout mPipelineLayout = VK_NULL_HANDLE;

		vk::raii::Pipeline mPipeline = VK_NULL_HANDLE;

		Ref<ShaderProgram> mProgram;

		Scope<VulkanShader> mBackendShader;
	};
} // namespace BHive