#include "gfx/RenderCommand.h"
#include "VulkanPipeline.h"
#include "VulkanRendererAPI.h"
#include "VulkanGraphicsContext.h"
#include "VulkanSwapChain.h"

namespace BHive
{

	VulkanPipeline::VulkanPipeline()
		: mDevice(VulkanCore::GetLogicalDevice())

	{
	}

	void VulkanPipeline::Init(const Configuration &configuration)
	{	
		mConfigration = static_cast<const FVulkanPipelineConfigInfo &>(configuration);

		std::vector dynamicStates = {vk::DynamicState::eViewport, vk::DynamicState::eScissor, vk::DynamicState::eLineWidth, vk::DynamicState::ePrimitiveTopologyEXT, vk::DynamicState::eVertexInputEXT};

		vk::PipelineDynamicStateCreateInfo dynamicStateInfo({}, dynamicStates);

		vk::GraphicsPipelineCreateInfo pipeline_info{};
		pipeline_info
			.setStages(mConfigration.ShaderCreateInfos)
			.setPVertexInputState(&mConfigration.InputState)
			.setPInputAssemblyState(&mConfigration.InputAssembly)
			.setPViewportState(&mConfigration.ViewportState)
			.setPRasterizationState(&mConfigration.Rasterazation)
			.setPMultisampleState(&mConfigration.MultiSampling)
			.setPColorBlendState(&mConfigration.ColorBlend)
			.setPDynamicState(&dynamicStateInfo)
			.setLayout(mConfigration.Layout)
			.setRenderPass(mConfigration.RenderPass)
			.setSubpass(mConfigration.SubPass)
			.setPNext(mConfigration.Next);


		mPipeline = vk::raii::Pipeline(mDevice, nullptr, pipeline_info);
	}

	void VulkanPipeline::Bind()
	{
		auto cmd = [=](const FVulkanFrameData &data) 
			{
				data.CommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, mPipeline); 
			};

		RenderCommand::GetAPI<VulkanRendererAPI>()->SubmitCommand(cmd);
	}

	void VulkanPipeline::UnBind()
	{
	}

	FVulkanPipelineConfigInfo VulkanPipeline::GetDefaultConfigInfo()
	{
		FVulkanPipelineConfigInfo config{};

		config.InputState.setVertexAttributeDescriptionCount(0).setVertexBindingDescriptionCount(0);

		config.ViewportState.setViewportCount(1).setScissorCount(1);

		config.Rasterazation.setDepthClampEnable(false)
			.setRasterizerDiscardEnable(false)
			.setPolygonMode(vk::PolygonMode::eFill)
			.setCullMode(vk::CullModeFlagBits::eBack)
			.setFrontFace(vk::FrontFace::eCounterClockwise)
			.setDepthBiasEnable(false)
			.setDepthBiasSlopeFactor(1.0f)
			.setLineWidth(1.0f);

		config.ColorBlendAttachment.setBlendEnable(false)
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