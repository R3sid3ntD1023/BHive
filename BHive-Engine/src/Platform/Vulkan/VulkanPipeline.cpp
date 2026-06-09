#include "gfx/RenderCommand.h"
#include "VulkanPipeline.h"
#include "VulkanRendererAPI.h"
#include "VulkanConverters.h"
#include "VulkanShader.h"
#include "gfx/shader/ShaderProgram.h"
#include "gfx/ISetManager.h"
#include "systems/GlobalSetRegistry.h"
#include "systems/MaterialSetRegistry.h"
#include "gfx/renderers/Renderer.h"

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

	Ref<FVulkanPipelineConfigInfo> Convert(const Pipeline::GraphicsPipelineState& state)
	{
		auto config = CreateRef<FVulkanPipelineConfigInfo>();

		config->InputState.setVertexAttributeDescriptionCount(0).setVertexBindingDescriptionCount(0);

		config->ViewportState.setViewportCount(1).setScissorCount(1);

		config->Rasterazation.setDepthClampEnable(VK_FALSE)
			.setRasterizerDiscardEnable(VK_FALSE)
			.setCullMode(state.Raster.CullEnabled ? ToVkCull(state.Raster.CullMode) : vk::CullModeFlagBits::eNone)
			.setPolygonMode(ToVkPolygon(state.Raster.FillMode))
			.setFrontFace(ToVkFrontFace(state.Raster.FrontFace))
			.setDepthBiasEnable(VK_FALSE)
			.setDepthBiasSlopeFactor(1.0f)
			.setLineWidth(1.0f);

		config->ColorBlendAttachment.setBlendEnable(state.Blend.Enabled)
			.setSrcColorBlendFactor(ToVkBlendFactor(state.Blend.SrcColor))
			.setDstColorBlendFactor(ToVkBlendFactor(state.Blend.DstColor))
			.setColorBlendOp(ToVkBlendOp(state.Blend.ColorOp))
			.setSrcAlphaBlendFactor(ToVkBlendFactor(state.Blend.SrcAlpha))
			.setDstAlphaBlendFactor(ToVkBlendFactor(state.Blend.DstAlpha))
			.setAlphaBlendOp(ToVkBlendOp(state.Blend.AlphaOp))
			.setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);

		config->InputAssembly.setTopology(ToVkTopology(state.DrawMode));

		config->ColorBlend.setLogicOpEnable(VK_FALSE).setLogicOp(vk::LogicOp::eCopy).setAttachments(config->ColorBlendAttachment);

		config->MultiSampling.setRasterizationSamples(vk::SampleCountFlagBits::e1).setSampleShadingEnable(VK_FALSE);

		config->DepthStencil.setDepthTestEnable(state.Depth.DepthTest)
			.setDepthWriteEnable(state.Depth.DepthWrite)
			.setDepthCompareOp(ToVkCompare(state.Depth.DepthCompare))
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

	void VulkanPipeline::Init(const GraphicsPipelineState& state)
	{	
		mProgram = state.ShaderProgram;

		ASSERT(mProgram)

		mShader = CreateScope<VulkanShader>();
		mShader->Init(mProgram->GetAssetRef());

		auto config = Convert(state);

		std::vector<vk::PipelineShaderStageCreateInfo> shader_create_infos;
		auto &modules = mShader->GetModules();
		for (auto& [stage, module] : modules)
		{
			vk::PipelineShaderStageCreateInfo info({} , ToSingleVkStage(stage), *module, "main");
			shader_create_infos.emplace_back(info);
		}

		auto& push_constant_ranges = mShader->GetPushConstantRanges();
		auto &layouts_in = mShader->GetLayouts();
		auto maxSet = mShader->GetMaxSet();

		std::vector<vk::DescriptorSetLayout> layouts_out(maxSet + 1, VK_NULL_HANDLE);

		for (uint32_t set = 0; set <= maxSet; set++)
		{
			if (layouts_in.contains(set))
				layouts_out[set] = *layouts_in.at(set);
			else
			{
				vk::DescriptorSetLayoutCreateInfo empty_info{};
				layouts_out[set] = mDevice.createDescriptorSetLayout(empty_info);
			}
		}
			
		vk::PipelineLayoutCreateInfo pipeline_layout_create_info({}, layouts_out, push_constant_ranges );
		mPipelineLayout = mDevice.createPipelineLayout(pipeline_layout_create_info);

		std::vector dynamicStates = {vk::DynamicState::eViewport, vk::DynamicState::eScissor, vk::DynamicState::eLineWidth, vk::DynamicState::ePrimitiveTopologyEXT, vk::DynamicState::eVertexInputEXT};

		vk::PipelineDynamicStateCreateInfo dynamicStateInfo({}, dynamicStates);

		std::vector<vk::Format> color_attachment_formats;
		vk::Format depth_attachment_format = ToVkFormat(state.DepthAttachmentFormat);

		for (auto &format : state.ColorAttachmentFormats)
			color_attachment_formats.emplace_back(ToVkFormat(format));

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

		//create sets
		if (mShader->HasSet(BATCH_SET_INDEX))
		{
			mBatchSetManager = RenderCommand::GetGraphicsAPI()->CreateSetManager(this, BATCH_SET_INDEX);
			mBatchSetManager->SetBuffer(1, Renderer::Get().GetModelBuffer().GetObjectBuffer());
		}

		RenderCommand::GetGraphicsAPI<VulkanRendererAPI>()->OnPipelineCreated(this);

		mBindPoint = vk::PipelineBindPoint::eGraphics;
	}

	void VulkanPipeline::Init(const ComputePipelineState &state)
	{

		mProgram = state.ShaderProgram;

		mShader = CreateScope<VulkanShader>();
		mShader->Init(mProgram->GetAssetRef());

		auto &modules = mShader->GetModules();
		auto has_compute_stage = modules.contains(EShaderStage::Compute);
		if (!has_compute_stage)
			return;

		auto& module = modules.at(EShaderStage::Compute);
		vk::PipelineShaderStageCreateInfo shader_create_info({}, vk::ShaderStageFlagBits::eCompute, *module, "main");

		auto &push_constant_ranges = mShader->GetPushConstantRanges();
		auto &layouts_in = mShader->GetLayouts();
		auto maxSet = mShader->GetMaxSet();

		std::vector<vk::DescriptorSetLayout> layouts_out(maxSet + 1, VK_NULL_HANDLE);

		for (uint32_t set = 0; set <= maxSet; set++)
		{
			if (layouts_in.contains(set))
				layouts_out[set] = *layouts_in.at(set);
			else
			{
				vk::DescriptorSetLayoutCreateInfo empty_info{};
				layouts_out[set] = mDevice.createDescriptorSetLayout(empty_info);
			}
		}

		vk::PipelineLayoutCreateInfo pipeline_layout_create_info({}, layouts_out, push_constant_ranges);
		mPipelineLayout = mDevice.createPipelineLayout(pipeline_layout_create_info);

		vk::ComputePipelineCreateInfo createInfo{};
		createInfo.setStage(shader_create_info);
		createInfo.setLayout(mPipelineLayout);

		mPipeline = vk::raii::Pipeline(mDevice, nullptr, createInfo);

		mBindPoint = vk::PipelineBindPoint::eCompute;
	}

	void VulkanPipeline::Bind()
	{
		auto api = Renderer::Get().GetGraphicsAPI<VulkanRendererAPI>();
		auto& shader = *mShader;
		auto& set_hashes = shader.GetSetHashes();
		auto &refl = mProgram->GetRefl();
		auto& layout = mPipelineLayout;
		auto maxSet = mShader->GetMaxSet();
		uint64_t h0 = set_hashes.at(GLOBAL_SET_INDEX);

		RenderCommand::SubmitCommand("Bind Pipeline && Descriptor Sets",
			[=, &layout](IRendererContext &ctx) 
			{
				auto &vk_ctx = CastRef<FVulkanRendererContext>(ctx);

				
				if (mObjectSetManager)
					mObjectSetManager->Update(vk_ctx.Frame);

				if (mBatchSetManager)
				{
					mBatchSetManager->Update(vk_ctx.Frame);
				}

				vk_ctx.CommandBuffer.bindPipeline(mBindPoint, mPipeline); 

				auto &registry = GetSubSystem<GlobalSetRegistry>();

				auto globalManager = registry.Find(h0);

				if (globalManager)
				{
					auto set = globalManager->GetNativeSet(vk_ctx.Frame).As<vk::DescriptorSet>();
					//const auto dynamicOffset = vk_ctx.ViewIndex * sizeof(FView);

					vk_ctx.CommandBuffer.bindDescriptorSets(mBindPoint, layout, GLOBAL_SET_INDEX, *set, {});
				}
					

				if (mObjectSetManager)
				{
					auto set = mObjectSetManager->GetNativeSet(vk_ctx.Frame).As<vk::DescriptorSet>();
					vk_ctx.CommandBuffer.bindDescriptorSets(mBindPoint, layout, OBJECT_SET_INDEX, *set, {});
				}

				if (mBatchSetManager)
				{
					
					auto set = mBatchSetManager->GetNativeSet(vk_ctx.Frame).As<vk::DescriptorSet>();
					vk_ctx.CommandBuffer.bindDescriptorSets(mBindPoint, layout, BATCH_SET_INDEX, *set, {});
				}
			});
	}

	void VulkanPipeline::BindImmediate(vk::CommandBuffer cmd)
	{
		auto &shader = *mShader;
		auto &set_hashes = shader.GetSetHashes();
		auto &refl = mProgram->GetRefl();
		auto &layout = mPipelineLayout;
		auto maxSet = mShader->GetMaxSet();
		uint64_t h0 = set_hashes.at(GLOBAL_SET_INDEX);

		if (mObjectSetManager)
			mObjectSetManager->Update(0);

		if (mBatchSetManager)
		{
			mBatchSetManager->Update(0);
		}

		cmd.bindPipeline(mBindPoint, mPipeline);

		auto &registry = GetSubSystem<GlobalSetRegistry>();

		auto globalManager = registry.Find(h0);

		if (globalManager)
		{
			auto set = globalManager->GetNativeSet(0).As<vk::DescriptorSet>();

			cmd.bindDescriptorSets(mBindPoint, layout, GLOBAL_SET_INDEX, *set, {});
		}

		if (mObjectSetManager)
		{
			auto set = mObjectSetManager->GetNativeSet(0).As<vk::DescriptorSet>();
			cmd.bindDescriptorSets(mBindPoint, layout, OBJECT_SET_INDEX, *set, {});
		}

		if (mBatchSetManager)
		{

			auto set = mBatchSetManager->GetNativeSet(0).As<vk::DescriptorSet>();
			cmd.bindDescriptorSets(mBindPoint, layout, BATCH_SET_INDEX, *set, {});
		}
	}

	Ref<ShaderProgram> VulkanPipeline::GetShaderProgram() const
	{
		return mProgram;
	}

	vk::DescriptorSetLayout VulkanPipeline::GetSetLayout(uint32_t set) const
	{
		return mShader->GetDescriptorSetLayout(set);
	}


	void VulkanPipeline::SetObjectSetManager(ISetManager *manager)
	{
		mObjectSetManager = manager;
	}


} // namespace BHive