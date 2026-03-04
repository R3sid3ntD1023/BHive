#include "gfx/RenderCommand.h"
#include "VulkanPipeline.h"
#include "VulkanRendererAPI.h"
#include "VulkanConverters.h"
#include "VulkanShader.h"
#include "gfx/shader/ShaderProgram.h"
#include "gfx/ISetManager.h"

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

	void VulkanPipeline::Init(const PipelineState& state)
	{	
		mProgram = state.ShaderProgram;

		ASSERT(mProgram)

		const auto &asset = mProgram->GetAsset();
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
	}

	void VulkanPipeline::Bind()
	{
		auto api = RenderCommand::GetRendererAPI<VulkanRendererAPI>();
		auto& shader = *mShader;
		auto& set_hashes = shader.GetSetHashes();
		auto &refl = mProgram->GetRefl();
		auto& layout = mPipelineLayout;
		auto maxSet = mShader->GetMaxSet();

		RenderCommand::SubmitResourceUpdate(
			[=, &shader, &set_hashes, &refl](IRendererContext &ctx)
			{
				auto &vk_ctx = CastRef<FVulkanRendererContext>(ctx);
				if (set_hashes.contains(0))
				{
					auto global_set = refl.Sets.at(GLOBAL_SET_INDEX);
					api->UpdateGlobalSet(this, vk_ctx.Frame);
				}

				if (mMaterialSetManager)
					mMaterialSetManager->Update(vk_ctx.Frame);

				if (mObjectSetManager)
					mObjectSetManager->Update(vk_ctx.Frame);

				if (mBatchSetManager)
					mBatchSetManager->Update(vk_ctx.Frame);
			});

		auto &pass = RenderCommand::GetActivePass();
		pass.CommandList.Push("Bind Pipeline && Descriptor Sets",
			[=, &layout](IRendererContext &ctx) 
			{
				auto &vk_ctx = CastRef<FVulkanRendererContext>(ctx);
				vk_ctx.CommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, mPipeline); 

				if (set_hashes.contains(0))
				{
					uint64_t h0 = set_hashes.at(GLOBAL_SET_INDEX);
					auto set = api->GetGlobalSet(h0, vk_ctx.Frame);
					vk_ctx.CommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, layout, GLOBAL_SET_INDEX, **set, {});
				}
					
				if (mMaterialSetManager)
				{
					auto set = mMaterialSetManager->GetNativeSet(vk_ctx.Frame).As<vk::raii::DescriptorSet>();
					vk_ctx.CommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, layout, MATERIAL_SET_INDEX, **set, {});
				}

				if (mObjectSetManager)
				{
					auto set = mObjectSetManager->GetNativeSet(vk_ctx.Frame).As<vk::raii::DescriptorSet>();
					vk_ctx.CommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, layout, OBJECT_SET_INDEX, **set, {});
				}

				if (mBatchSetManager)

				{
					auto set = mBatchSetManager->GetNativeSet(vk_ctx.Frame).As<vk::raii::DescriptorSet>();
					vk_ctx.CommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, layout, BATCH_SET_INDEX, **set, {});
				}
			});
	}

	Ref<ShaderProgram> VulkanPipeline::GetShaderProgram() const
	{
		return mProgram;
	}

	vk::DescriptorSetLayout VulkanPipeline::GetSetLayout(uint32_t set) const
	{
		return mShader->GetDescriptorSetLayout(set);
	}

	void VulkanPipeline::SetMaterialSetManager(ISetManager *manager)
	{
		mMaterialSetManager = manager;
	}

	void VulkanPipeline::SetObjectSetManager(ISetManager *manager)
	{
		mObjectSetManager = manager;
	}

	void VulkanPipeline::SetBatchSetManager(ISetManager *manager)
	{
		mBatchSetManager = manager;
	}

} // namespace BHive