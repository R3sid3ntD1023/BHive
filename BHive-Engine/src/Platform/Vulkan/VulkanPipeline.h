#pragma once

#include "core/Core.h"
#include "gfx/Pipeline.h"
#include "VulkanCore.h"

namespace BHive
{
	class Shader;

	struct FVulkanPipelineConfigInfo : public Pipeline::Configuration
	{
		vk::Viewport Viewport;
		vk::Rect2D Scissor;
		vk::PipelineVertexInputStateCreateInfo InputState;
		vk::PipelineViewportStateCreateInfo ViewportState;
		vk::PipelineInputAssemblyStateCreateInfo InputAssembly;
		vk::PipelineRasterizationStateCreateInfo Rasterazation;
		vk::PipelineMultisampleStateCreateInfo MultiSampling;
		vk::PipelineColorBlendAttachmentState ColorBlendAttachment;
		vk::PipelineColorBlendStateCreateInfo ColorBlend;
		vk::PipelineDepthStencilStateCreateInfo DepthStencil;
		vk::PipelineLayout Layout = VK_NULL_HANDLE;
		vk::RenderPass RenderPass = VK_NULL_HANDLE;
		uint32_t SubPass = 0;
		void *Next = nullptr;
		std::vector<vk::PipelineShaderStageCreateInfo> ShaderCreateInfos;
	};

	class BHIVE_API VulkanPipeline : public Pipeline
	{
	public:
		VulkanPipeline();

		virtual void Init(const Configuration &configuration) override;

		virtual void Bind() override;

		virtual void UnBind() override;

		const vk::Pipeline &operator*() const { return mPipeline; }

		static FVulkanPipelineConfigInfo GetDefaultConfigInfo();

	private:
		vk::raii::Device &mDevice;

		vk::raii::Pipeline mPipeline = VK_NULL_HANDLE;

		FVulkanPipelineConfigInfo mConfigration;
	};
} // namespace BHive