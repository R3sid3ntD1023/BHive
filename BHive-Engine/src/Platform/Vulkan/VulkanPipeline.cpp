#include "gfx/RenderCommand.h"
#include "VulkanPipeline.h"
#include "VulkanRendererAPI.h"
#include "VulkanConverters.h"
#include "VulkanShader.h"
#include "gfx/shader/ShaderProgram.h"

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
		vk::RenderPass RenderPass = VK_NULL_HANDLE;
		uint32_t SubPass = 0;
	};

	Ref<FVulkanPipelineConfigInfo> Convert(const Pipeline::PipelineState& state)
	{
		auto config = CreateRef<FVulkanPipelineConfigInfo>();

		config->InputState.setVertexAttributeDescriptionCount(0).setVertexBindingDescriptionCount(0);

		config->ViewportState.setViewportCount(1).setScissorCount(1);

		config->Rasterazation.setDepthClampEnable(VK_FALSE)
			.setRasterizerDiscardEnable(VK_FALSE)
			.setCullMode(state.Raster.CullEnabled ? Vulkan::ToVkCull(state.Raster.CullMode) : vk::CullModeFlagBits::eNone)
			.setPolygonMode(Vulkan::ToVkPolygon(state.Raster.FillMode))
			.setFrontFace(Vulkan::ToVkFrontFace(state.Raster.FrontFace))
			.setDepthBiasEnable(VK_FALSE)
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

		config->ColorBlend.setLogicOpEnable(VK_FALSE).setLogicOp(vk::LogicOp::eCopy).setAttachments(config->ColorBlendAttachment);

		config->MultiSampling.setRasterizationSamples(vk::SampleCountFlagBits::e1).setSampleShadingEnable(VK_FALSE);

		config->DepthStencil.setDepthTestEnable(state.Depth.DepthTest)
			.setDepthWriteEnable(state.Depth.DepthWrite)
			.setDepthCompareOp(Vulkan::ToVkCompare(state.Depth.DepthCompare))
			.setStencilTestEnable(VK_TRUE);
	
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

	void VulkanPipeline::Init(const PipelineState& state)
	{	
		mProgram = state.ShaderProgram;

		ASSERT(mProgram)

		const auto &asset = mProgram->GetAsset();
		mBackendShader = CreateScope<VulkanShader>();
		mBackendShader->Init(mProgram->GetAssetRef());

		auto config = Convert(state);

		std::vector<vk::PipelineShaderStageCreateInfo> shader_create_infos;
		auto &modules = mBackendShader->GetModules();
		for (auto& [stage, module] : modules)
		{
			vk::PipelineShaderStageCreateInfo info({} , Vulkan::ToSingleVkStage(stage), *module, "main");
			shader_create_infos.emplace_back(info);
		}

		auto& push_constant_ranges = mBackendShader->GetPushConstantRanges();
		auto &layout_in = mBackendShader->GetDescriptorSetLayouts();
		std::vector<vk::DescriptorSetLayout> layouts_out;
		layouts_out.reserve(layout_in.size());

		std::transform(layout_in.begin(), layout_in.end(), std::back_inserter(layouts_out), [](const vk::raii::DescriptorSetLayout &l) { return *l; });

		vk::PipelineLayoutCreateInfo pipeline_layout_create_info({}, layouts_out, push_constant_ranges );
		mPipelineLayout = mDevice.createPipelineLayout(pipeline_layout_create_info);

		std::vector dynamicStates = {vk::DynamicState::eViewport, vk::DynamicState::eScissor, vk::DynamicState::eLineWidth, vk::DynamicState::ePrimitiveTopologyEXT, vk::DynamicState::eVertexInputEXT};

		vk::PipelineDynamicStateCreateInfo dynamicStateInfo({}, dynamicStates);

		std::vector<vk::Format> color_attachment_formats;
		vk::Format depth_attachment_format = Vulkan::ToVkFormat(state.DepthAttachmentFormat);

		for (auto &format : state.ColorAttachmentFormats)
			color_attachment_formats.emplace_back(Vulkan::ToVkFormat(format));

		vk::PipelineRenderingCreateInfo rendering_info{};
		rendering_info.setViewMask(0).setColorAttachmentCount(color_attachment_formats.size()).setColorAttachmentFormats(color_attachment_formats)
			.setDepthAttachmentFormat(depth_attachment_format);
		

		vk::GraphicsPipelineCreateInfo pipeline_info{};
		pipeline_info
			.setStages(shader_create_infos)
			.setPVertexInputState(&config->InputState)
			.setPInputAssemblyState(&config->InputAssembly)
			.setPViewportState(&config->ViewportState)
			.setPRasterizationState(&config->Rasterazation)
			.setPMultisampleState(&config->MultiSampling)
			.setPColorBlendState(&config->ColorBlend)
			.setPDepthStencilState(&config->DepthStencil)
			.setPDynamicState(&dynamicStateInfo)
			.setLayout(mPipelineLayout)
			.setRenderPass(config->RenderPass)
			.setSubpass(config->SubPass)
			.setPNext(&rendering_info);


		mPipeline = vk::raii::Pipeline(mDevice, nullptr, pipeline_info);
	}

	void VulkanPipeline::Bind()
	{
		auto cmd = [=](const FVulkanFrame &data) 
			{
				data.CommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, mPipeline); 
			};

		RenderCommand::GetRendererAPI<VulkanRendererAPI>()->SubmitCommand(cmd);
	}

	void VulkanPipeline::UnBind()
	{
	}

	Ref<ShaderProgram> VulkanPipeline::GetShaderProgram() const
	{
		return mProgram;
	}

	const std::vector<vk::raii::DescriptorSetLayout> &VulkanPipeline::GetDescriptorLayouts() const
	{
		return mBackendShader->GetDescriptorSetLayouts();
	}


} // namespace BHive