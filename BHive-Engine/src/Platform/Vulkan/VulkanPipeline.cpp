#include "gfx/RenderCommand.h"
#include "VulkanPipeline.h"
#include "VulkanRendererAPI.h"
#include "VulkanConverters.h"

namespace BHive
{
	struct FVulkanPipelineConfigInfo
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

	Ref<FVulkanPipelineConfigInfo> Convert(const Pipeline::PipelineState& state)
	{
		auto config = CreateRef<FVulkanPipelineConfigInfo>();

		config->InputState.setVertexAttributeDescriptionCount(0).setVertexBindingDescriptionCount(0);

		config->ViewportState.setViewportCount(1).setScissorCount(1);

		config->Rasterazation.setDepthClampEnable(false)
			.setRasterizerDiscardEnable(false)
			.setPolygonMode(Vulkan::ToVkPolygon(state.Raster.FillMode))
			.setCullMode(Vulkan::ToVkCull(state.Raster.CullMode))
			.setFrontFace(Vulkan::ToVkFrontFace(state.Raster.FrontFace))
			.setDepthBiasEnable(false)
			.setDepthBiasSlopeFactor(1.0f)
			.setLineWidth(1.0f);

		config->ColorBlendAttachment.setBlendEnable(state.Blend.Enabled)
			.setSrcColorBlendFactor(Vulkan::ToVkBlendFactor(state.Blend.SrcColor))
			.setDstColorBlendFactor(Vulkan::ToVkBlendFactor(state.Blend.DstColor))
			.setColorBlendOp(Vulkan::ToVkBlendOp(state.Blend.ColorOp))
			.setSrcAlphaBlendFactor(Vulkan::ToVkBlendFactor(state.Blend.SrcAlpha))
			.setDstAlphaBlendFactor(Vulkan::ToVkBlendFactor(state.Blend.DstAlpha))
			.setAlphaBlendOp(Vulkan::ToVkBlendOp(state.Blend.AlphaOp))
			.setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);

		config->InputAssembly.setTopology(Vulkan::ToVkTopology(state.DrawMode));

		config->ColorBlend.setLogicOpEnable(false).setLogicOp(vk::LogicOp::eCopy).setAttachments(config->ColorBlendAttachment);

		config->MultiSampling.setRasterizationSamples(vk::SampleCountFlagBits::e1).setSampleShadingEnable(false);

		return config;
	}

	VulkanPipeline::VulkanPipeline()
		: mDevice(VulkanBackend::GetLogicalDevice())

	{
	}

	VulkanPipeline::~VulkanPipeline()
	{
		LOG_TRACE("VulkanPipeline Destructor Called")

		//mPipeline.clear();
	}

	void VulkanPipeline::Init(const PipelineState& configuration)
	{	
		auto config = Convert(configuration);

		std::vector dynamicStates = {vk::DynamicState::eViewport, vk::DynamicState::eScissor, vk::DynamicState::eLineWidth, vk::DynamicState::ePrimitiveTopologyEXT, vk::DynamicState::eVertexInputEXT};

		vk::PipelineDynamicStateCreateInfo dynamicStateInfo({}, dynamicStates);

		vk::GraphicsPipelineCreateInfo pipeline_info{};
		pipeline_info
			.setStages(config->ShaderCreateInfos)
			.setPVertexInputState(&config->InputState)
			.setPInputAssemblyState(&config->InputAssembly)
			.setPViewportState(&config->ViewportState)
			.setPRasterizationState(&config->Rasterazation)
			.setPMultisampleState(&config->MultiSampling)
			.setPColorBlendState(&config->ColorBlend)
			.setPDepthStencilState(&config->DepthStencil)
			.setPDynamicState(&dynamicStateInfo)
			.setLayout(config->Layout)
			.setRenderPass(config->RenderPass)
			.setSubpass(config->SubPass)
			.setPNext(config->Next);


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

} // namespace BHive