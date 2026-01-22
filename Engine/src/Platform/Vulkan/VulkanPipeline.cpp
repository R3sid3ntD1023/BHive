#include "gfx/RenderCommand.h"
#include "VulkanPipeline.h"
#include "VulkanRendererAPI.h"

namespace BHive
{

	VulkanPipeline::VulkanPipeline()
		: mDevice(VulkanCore::GetLogicalDevice())

	{
	}

	void VulkanPipeline::Init(const Configuration &configuration)
	{
		auto &config = static_cast<const FVulkanPipelineConfigInfo &>(configuration);

		static const std::vector dynamicStates = {
			vk::DynamicState::eViewport, vk::DynamicState::eScissor, vk::DynamicState::eVertexInputEXT, vk::DynamicState::ePrimitiveTopologyEXT, vk::DynamicState::eLineWidth};

		vk::PipelineDynamicStateCreateInfo dynamicStateInfo({}, dynamicStates);

		vk::GraphicsPipelineCreateInfo pipeline_info{};
		pipeline_info.setStages(config.ShaderCreateInfos)
			.setPVertexInputState(nullptr)
			.setPInputAssemblyState(&config.InputAssembly)
			.setPViewportState(&config.ViewportState)
			.setPRasterizationState(&config.Rasterazation)
			.setPMultisampleState(&config.MultiSampling)
			.setPColorBlendState(&config.ColorBlend)
			.setPDynamicState(&dynamicStateInfo)
			.setLayout(config.Layout)
			.setRenderPass(config.RenderPass)
			.setSubpass(config.SubPass)
			.setPNext(config.Next);

		mPipeline = mDevice.createGraphicsPipeline(nullptr, pipeline_info);
	}

	void VulkanPipeline::Bind()
	{
		vk::Pipeline pipelineHandle = *mPipeline;
		auto cmd = [=](const FVulkanFrameData &data) { data.CommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipelineHandle); };

		RenderCommand::GetAPI<VulkanRendererAPI>()->SubmitCommand(cmd);
	}

	void VulkanPipeline::UnBind()
	{
	}

	FVulkanPipelineConfigInfo VulkanPipeline::GetDefaultConfigInfo(uint32_t width, uint32_t height)
	{
		FVulkanPipelineConfigInfo config{};

		config.Viewport.setX(0.0f).setY(0.0f).setWidth((float)width).setHeight((float)height).setMinDepth(0.f).setMaxDepth(1.f);

		config.Scissor.setOffset({0, 0}).setExtent({width, height});

		config.ViewportState.setViewportCount(1).setScissorCount(1);

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

		config.ColorBlend.setLogicOpEnable(false).setLogicOp(vk::LogicOp::eCopy).setAttachments(config.ColorBlendAttachment);

		config.MultiSampling.setRasterizationSamples(vk::SampleCountFlagBits::e1).setSampleShadingEnable(false);

		return config;
	}

} // namespace BHive