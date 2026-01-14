#include "Shader.h"
#include "VulkanPipeline.h"

namespace BHive
{


	void VulkanPipeline::Init(vk::raii::Device& device, const std::vector<vk::PipelineShaderStageCreateInfo> &shaders, const FPipelineConfigInfo &config)
	{
		static const std::vector dynamicStates = {
			vk::DynamicState::eViewport, 
			vk::DynamicState::eScissor, 
			vk::DynamicState::eVertexInputEXT, 
			vk::DynamicState::ePrimitiveTopologyEXT,
			vk::DynamicState::eLineWidth
		};

		vk::PipelineDynamicStateCreateInfo dynamicStateInfo({} , dynamicStates);

		vk::GraphicsPipelineCreateInfo pipeline_info{};
		pipeline_info.setPNext(config.Next)
			.setStageCount(shaders.size())
			.setPStages(shaders.data())
			.setPVertexInputState(nullptr)
			.setPInputAssemblyState(&config.InputAssembly)
			.setPViewportState(&config.ViewportState)
			.setPRasterizationState(& config.Rasterazation)
			.setPMultisampleState(& config.MultiSampling)
			.setPColorBlendState(&config.ColorBlend)
			.setPDynamicState(& dynamicStateInfo)
			.setLayout(config.Layout)
			.setRenderPass(config.RenderPass)
			.setSubpass(config.SubPass);

		mPipeline = device.createGraphicsPipeline(nullptr, pipeline_info);
	}

	FPipelineConfigInfo VulkanPipeline::GetDefaultConfigInfo(uint32_t width, uint32_t height)
	{
		FPipelineConfigInfo config{};

		config.Viewport.setX(0.0f)
		.setY(0.0f)
		.setWidth((float)width)
		.setHeight((float)height)
		.setMinDepth(0.f)
		.setMaxDepth(1.f);

		config.Scissor.setOffset(vk::Offset2D{0, 0})
			.setExtent(vk::Extent2D{width, height});

		config.ViewportState.setViewportCount(1)
			.setScissorCount(1);

		//config.InputAssembly.setTopology(vk::PrimitiveTopology::eTriangleList);

		config.Rasterazation.setDepthClampEnable(false).setRasterizerDiscardEnable(false)
			.setPolygonMode(vk::PolygonMode::eFill)
			.setCullMode(vk::CullModeFlagBits::eBack)
			.setFrontFace (vk::FrontFace::eCounterClockwise)
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

		
		config.ColorBlend.setLogicOpEnable(false)
		.setLogicOp(vk::LogicOp::eCopy)
		.setAttachmentCount(1).setPAttachments(& config.ColorBlendAttachment);

		config.MultiSampling.setRasterizationSamples(vk::SampleCountFlagBits::e1).setSampleShadingEnable(false);

		return config;
	}

} // namespace BHive