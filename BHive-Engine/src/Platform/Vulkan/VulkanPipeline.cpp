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

	VulkanPipeline::~VulkanPipeline()
	{
		LOG_TRACE("VulkanPipeline Destructor Called")

		//mPipeline.clear();
	}

	void VulkanPipeline::Init(const Ref<Configuration >& configuration)
	{	
		mConfiguration = Cast<FVulkanPipelineConfigInfo>(configuration);

		std::vector dynamicStates = {vk::DynamicState::eViewport, vk::DynamicState::eScissor, vk::DynamicState::eLineWidth, vk::DynamicState::ePrimitiveTopologyEXT, vk::DynamicState::eVertexInputEXT};

		vk::PipelineDynamicStateCreateInfo dynamicStateInfo({}, dynamicStates);

		vk::GraphicsPipelineCreateInfo pipeline_info{};
		pipeline_info
			.setStages(mConfiguration->ShaderCreateInfos)
			.setPVertexInputState(&mConfiguration->InputState)
			.setPInputAssemblyState(&mConfiguration->InputAssembly)
			.setPViewportState(&mConfiguration->ViewportState)
			.setPRasterizationState(&mConfiguration->Rasterazation)
			.setPMultisampleState(&mConfiguration->MultiSampling)
			.setPColorBlendState(&mConfiguration->ColorBlend)
			.setPDepthStencilState(&mConfiguration->DepthStencil)
			.setPDynamicState(&dynamicStateInfo)
			.setLayout(mConfiguration->Layout)
			.setRenderPass(mConfiguration->RenderPass)
			.setSubpass(mConfiguration->SubPass)
			.setPNext(mConfiguration->Next);


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

	Ref<FVulkanPipelineConfigInfo> VulkanPipeline::GetDefaultConfigInfo()
	{
		auto config = CreateRef<FVulkanPipelineConfigInfo>();

		ASSERT(config);

		config->InputState.setVertexAttributeDescriptionCount(0).setVertexBindingDescriptionCount(0);

		config->ViewportState.setViewportCount(1).setScissorCount(1);

		config->Rasterazation.setDepthClampEnable(false)
			.setRasterizerDiscardEnable(false)
			.setPolygonMode(vk::PolygonMode::eFill)
			.setCullMode(vk::CullModeFlagBits::eBack)
			.setFrontFace(vk::FrontFace::eCounterClockwise)
			.setDepthBiasEnable(false)
			.setDepthBiasSlopeFactor(1.0f)
			.setLineWidth(1.0f);

		config->ColorBlendAttachment.setBlendEnable(false)
			.setSrcColorBlendFactor(vk::BlendFactor::eSrcAlpha)
			.setDstColorBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha)
			.setColorBlendOp(vk::BlendOp::eAdd)
			.setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
			.setDstAlphaBlendFactor(vk::BlendFactor::eZero)
			.setAlphaBlendOp(vk::BlendOp::eAdd)
			.setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);

		config->InputAssembly.setTopology(vk::PrimitiveTopology::eTriangleList);

		config->ColorBlend.setLogicOpEnable(false).setLogicOp(vk::LogicOp::eCopy).setAttachments(config->ColorBlendAttachment);
		
		config->MultiSampling.setRasterizationSamples(vk::SampleCountFlagBits::e1).setSampleShadingEnable(false);

		return config;
	}

} // namespace BHive