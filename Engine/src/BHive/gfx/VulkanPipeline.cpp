#include "Shader.h"
#include "VulkanPipeline.h"

namespace BHive
{


	void VulkanPipeline::Init(vk::raii::Device& device, const std::vector<Ref<Shader>> &shaders, const FPipelineConfigInfo &config)
	{
		static const std::vector dynamicStates = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};

		vk::PipelineDynamicStateCreateInfo dynamicStateInfo{};
		dynamicStateInfo.pDynamicStates = dynamicStates.data();
		dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());

		for (auto& shader : shaders)
		{
			const auto& create_infos = shader->GetStageCreateInfos();
			mStages.insert(mStages.begin(), create_infos.begin(), create_infos.end());
		}

		vk::GraphicsPipelineCreateInfo pipeline_info{};
		pipeline_info.pNext = config.Next;
		pipeline_info.stageCount = static_cast<uint32_t>(mStages.size());
		pipeline_info.pStages = mStages.data();
		pipeline_info.pVertexInputState = &config.InputState;
		pipeline_info.pInputAssemblyState = &config.InputAssembly;
		pipeline_info.pViewportState = &config.ViewportState;
		pipeline_info.pRasterizationState = &config.Rasterazation;
		pipeline_info.pMultisampleState = &config.MultiSampling;
		pipeline_info.pColorBlendState = &config.ColorBlend;
		pipeline_info.pDynamicState = &dynamicStateInfo;
		pipeline_info.layout = config.Layout;
		pipeline_info.renderPass = config.RenderPass;
		pipeline_info.subpass = config.SubPass;

		mPipeline = device.createGraphicsPipeline(nullptr, pipeline_info);
	}

	FPipelineConfigInfo VulkanPipeline::GetDefaultConfigInfo(uint32_t width, uint32_t height)
	{
		FPipelineConfigInfo config{};

		config.Viewport.x = 0.f;
		config.Viewport.y = 0.f;
		config.Viewport.width = (float)width;
		config.Viewport.height = (float)height;
		config.Viewport.minDepth = 0.f;
		config.Viewport.maxDepth = 1.f;

		config.Scissor.offset = vk::Offset2D{0, 0};
		config.Scissor.extent = vk::Extent2D{width, height};

		config.ViewportState.viewportCount = 1;
		config.ViewportState.scissorCount = 1;

		config.InputAssembly.topology = vk::PrimitiveTopology::eTriangleList;

		config.Rasterazation.depthClampEnable = VK_FALSE;
		config.Rasterazation.rasterizerDiscardEnable = VK_FALSE;
		config.Rasterazation.polygonMode = vk::PolygonMode::eFill;
		config.Rasterazation.cullMode = vk::CullModeFlagBits::eBack;
		config.Rasterazation.frontFace = vk::FrontFace::eCounterClockwise;
		config.Rasterazation.depthBiasEnable = VK_FALSE;
		config.Rasterazation.depthBiasSlopeFactor = 1.0f;
		config.Rasterazation.lineWidth = 1.0f;

		config.ColorBlendAttachment.blendEnable = vk::True;
		config.ColorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
		config.ColorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
		config.ColorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;
		config.ColorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
		config.ColorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
		config.ColorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd;
		config.ColorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;

		
		config.ColorBlend.logicOpEnable = VK_FALSE;
		config.ColorBlend.logicOp = vk::LogicOp::eCopy;
		config.ColorBlend.attachmentCount = 1;
		config.ColorBlend.pAttachments = &config.ColorBlendAttachment;

		config.MultiSampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
		config.MultiSampling.sampleShadingEnable = VK_FALSE;

		return config;
	}

} // namespace BHive