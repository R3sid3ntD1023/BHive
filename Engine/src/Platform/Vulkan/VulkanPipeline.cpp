#include "gfx/RenderCommand.h"
#include "VulkanPipeline.h"

namespace BHive
{

	VulkanPipeline::VulkanPipeline(const FVulkanPipelineConfigInfo &configuration)
		: mDevice(VulkanCore::GetLogicalDevice()),
		  mConfiguration(configuration)

	{
	}

	void VulkanPipeline::Init()
	{
		static const std::vector dynamicStates = {
			vk::DynamicState::eViewport, vk::DynamicState::eScissor, vk::DynamicState::eVertexInputEXT, vk::DynamicState::ePrimitiveTopologyEXT, vk::DynamicState::eLineWidth};

		vk::PipelineDynamicStateCreateInfo dynamicStateInfo({}, dynamicStates);

		vk::GraphicsPipelineCreateInfo pipeline_info{};
		pipeline_info.setPNext(mConfiguration.Next)
			.setStages(mConfiguration.ShaderCreateInfos)
			.setPVertexInputState(nullptr)
			.setPInputAssemblyState(&mConfiguration.InputAssembly)
			.setPViewportState(&mConfiguration.ViewportState)
			.setPRasterizationState(&mConfiguration.Rasterazation)
			.setPMultisampleState(&mConfiguration.MultiSampling)
			.setPColorBlendState(&mConfiguration.ColorBlend)
			.setPDynamicState(&dynamicStateInfo)
			.setLayout(mConfiguration.Layout)
			.setRenderPass(mConfiguration.RenderPass)
			.setSubpass(mConfiguration.SubPass);

		mPipeline = mDevice.createGraphicsPipeline(nullptr, pipeline_info);
	}

	void VulkanPipeline::Bind()
	{
	}

	void VulkanPipeline::UnBind()
	{
	}

	FVulkanPipelineConfigInfo VulkanPipeline::GetDefaultConfigInfo(uint32_t width, uint32_t height)
	{
		FVulkanPipelineConfigInfo config{};

		config.Viewport.setX(0.0f).setY(0.0f).setWidth((float)width).setHeight((float)height).setMinDepth(0.f).setMaxDepth(1.f);

		config.Scissor.setOffset(vk::Offset2D{0, 0}).setExtent(vk::Extent2D{width, height});

		config.ViewportState.setViewportCount(1).setScissorCount(1);
		config.InputAssembly.setTopology(vk::PrimitiveTopology::eTriangleList);

		config.Rasterazation.setDepthClampEnable(false)
			.setRasterizerDiscardEnable(false)
			.setPolygonMode(vk::PolygonMode::eFill)
			.setCullMode(vk::CullModeFlagBits::eBack)
			.setFrontFace(vk::FrontFace::eCounterClockwise)
			.setDepthBiasEnable(false)
			.setDepthBiasSlopeFactor(1.0f)
			.setLineWidth(1.0f);

		config.ColorBlendAttachment.setBlendEnable(true)
			.setSrcColorBlendFactor(vk::BlendFactor::eSrcAlpha)
			.setDstColorBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha)
			.setColorBlendOp(vk::BlendOp::eAdd)
			.setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
			.setDstAlphaBlendFactor(vk::BlendFactor::eZero)
			.setAlphaBlendOp(vk::BlendOp::eAdd)
			.setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);

		config.InputAssembly.setTopology(vk::PrimitiveTopology::eTriangleList);

		config.ColorBlend.setLogicOpEnable(false).setLogicOp(vk::LogicOp::eCopy).setAttachmentCount(1).setPAttachments(&config.ColorBlendAttachment);

		config.MultiSampling.setRasterizationSamples(vk::SampleCountFlagBits::e1).setSampleShadingEnable(false);

		return config;
	}

} // namespace BHive