#pragma once

#include "core/Core.h"
#include "VulkanCore.h"

namespace BHive
{
	class Shader;

	struct FPipelineConfigInfo
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
	};

	class BHIVE_API VulkanPipeline
	{
	public:

		void Init(vk::raii::Device &device, const std::vector<vk::PipelineShaderStageCreateInfo> &shaders, const FPipelineConfigInfo &config);

		operator const vk::raii::Pipeline &() const { return mPipeline; }

		operator const vk::Pipeline &() const { return mPipeline; }

		static FPipelineConfigInfo GetDefaultConfigInfo(uint32_t width, uint32_t height);

	private:
		std::vector<vk::PipelineShaderStageCreateInfo> mStages;
		vk::raii::Pipeline mPipeline = VK_NULL_HANDLE;
	};
}