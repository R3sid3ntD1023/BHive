#pragma once

#include "core/Core.h"
#include "gfx/Pipeline.h"
#include  "VulkanBackend.h"

namespace BHive
{
	class Shader;

	struct FVulkanPipelineConfigInfo : public Pipeline::Configuration
	{
		vk::PipelineVertexInputStateCreateInfo InputState{};
		vk::PipelineViewportStateCreateInfo ViewportState{};
		vk::PipelineInputAssemblyStateCreateInfo InputAssembly{};
		vk::PipelineRasterizationStateCreateInfo Rasterazation{};
		vk::PipelineMultisampleStateCreateInfo MultiSampling{};
		vk::PipelineColorBlendAttachmentState ColorBlendAttachment{};
		vk::PipelineColorBlendStateCreateInfo ColorBlend{};
		vk::PipelineDepthStencilStateCreateInfo DepthStencil{};
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

		~VulkanPipeline();

		virtual void Init(const Ref<Configuration>& configuration) override;

		virtual void Bind() override;

		virtual void UnBind() override;

		const vk::Pipeline &operator*() const { return mPipeline; }

		static Ref<FVulkanPipelineConfigInfo> GetDefaultConfigInfo();

	private:
		vk::raii::Device &mDevice;

		vk::raii::Pipeline mPipeline = VK_NULL_HANDLE;

		Ref<FVulkanPipelineConfigInfo> mConfiguration;

	};
} // namespace BHive