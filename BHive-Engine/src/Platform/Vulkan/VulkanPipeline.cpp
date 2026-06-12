#include "gfx/RenderCommand.h"
#include "VulkanPipeline.h"
#include "VulkanRendererAPI.h"
#include "VulkanConverters.h"
#include "VulkanShader.h"
#include "gfx/shader/ShaderProgram.h"
#include "DescriptorSetManager.h"
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

		mBindPoint = vk::PipelineBindPoint::eGraphics;

		BindGlobalResources();
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

		BindGlobalResources();
	}

	void VulkanPipeline::Bind()
	{
		RenderCommand::SubmitCommand("Update sets -> Bind pipeline && sets",
			[=](IRendererContext &ctx) 
			{
				auto &vk_ctx = ctx.As<FVulkanRendererContext>();
				const auto frame = vk_ctx.Frame;
				vk_ctx.CommandBuffer.bindPipeline(mBindPoint, mPipeline); 

				for (auto& [setIndex, manager] : mSetManagers)
				{
					auto set = manager->GetNativeSet(frame).As<vk::DescriptorSet>();
					vk_ctx.CommandBuffer.bindDescriptorSets(mBindPoint, mPipelineLayout, setIndex, *set, {});
				}
			});
	}

	void VulkanPipeline::BindImmediate(vk::CommandBuffer cmd)
	{
		cmd.bindPipeline(mBindPoint, mPipeline);

		for (auto &[setIndex, manager] : mSetManagers)
		{
			auto set = manager->GetNativeSet(0).As<vk::DescriptorSet>();
			cmd.bindDescriptorSets(mBindPoint, mPipelineLayout, setIndex, *set, {});
		}
	}

	void VulkanPipeline::UpdateSets(uint32_t frame)
	{
		for (auto &[setIndex, manager] : mSetManagers)
			manager->Update(frame);
	}

	DescriptorSetManager *VulkanPipeline::GetOrCreateSet(uint32_t setIndex)
	{
		if (!mSetManagers.contains(setIndex))
		{
			auto api = RenderCommand::GetGraphicsAPI<VulkanRendererAPI>();
			vk::DescriptorPool pool = api->GetDescriptorPool();
			auto refl = mProgram->GetRefl();
			auto layout = mShader->GetDescriptorSetLayout(setIndex);

			auto manager = CreateRef<DescriptorSetManager>(VulkanBackend::GetLogicalDevice(), pool, layout, setIndex, refl);

			const auto& shaderName = mProgram->GetName();
			manager->SetDebugName(std::format("{}_Set{}", shaderName, setIndex));

			mSetManagers.emplace(setIndex, manager);
		}

		return mSetManagers.at(setIndex).get();
	}

	Ref<ShaderProgram> VulkanPipeline::GetShaderProgram() const
	{
		return mProgram;
	}

	vk::DescriptorSetLayout VulkanPipeline::GetSetLayout(uint32_t set) const
	{
		return mShader->GetDescriptorSetLayout(set);
	}

	void VulkanPipeline::BindGlobalResources()
	{
		const auto &setIndex = GLOBAL_SET_INDEX;
		auto &globals = Renderer::Get().GetGlobalResources();
		auto &bindings = mProgram->GetRefl().GetSetBindings(setIndex);
		const auto& shaderName = mProgram->GetName();
	
		if (bindings.empty())
		{
			LOG_INFO("GlobalSetRegistry: Shader '{}' has no global resources in set {}, skipping global binding.", shaderName, setIndex);
			return;
		}

		auto set = GetOrCreateSet(setIndex);

		for (auto &r : bindings)
		{
			const std::string semantic = r.Semantic.empty() ? r.name : r.Semantic;

			if (r.Semantic.empty())
			{
				LOG_ERROR(
					"Shader '{}' has resource '{}' in set {} binding {} with NO semantic tag.\n"
					"Add: // @semantic <Name> above the declaration.",
					shaderName, r.name, setIndex, r.binding);
			}

			auto *res = globals.Find(semantic);

			if (!res)
			{
				auto guess = globals.GuessSemanticFromName(semantic);
				if (!guess.empty())
				{
					LOG_WARN(
						"Shader '{}' variable '{}' requested semantic '{}', but it was not found.\n"
						"   Did you mean semantic '{}'?",
						shaderName, r.name, semantic, guess);
				}

				LOG_ERROR(
					"GlobalSet Binding Error:\n"
					"  Shader: {}\n"
					"  Set: {}\n"
					"  Binding: {}\n"
					"  Shader Variable: '{}'\n"
					"  Semantic Requested: '{}'\n"
					"  BUT GlobalResources does not contain this semantic.\n"
					"  Registered Global Semantics: {}",
					shaderName, setIndex, r.binding, r.name, semantic, globals.DebugListSemantics());

				ASSERT(false, "Missing global semantic")

				continue;
			}
			else if (res->IsBuffer() && IsBuffer(r.kind))
			{
				set->SetBuffer(r.binding, res->BufferRef);
				LOG_INFO("GlobalSet: bound BUFFER '{}' (semantic '{}') at set {}, binding {}", r.name, semantic, setIndex, r.binding);
				continue;
			}

			if (IsTexture(r.kind))
			{
				set->SetTexture(r.binding, res->TextureRef);
				LOG_INFO("GlobalSet: bound TEXTURE '{}' (semantic '{}') at set {}, binding {}", r.name, semantic, setIndex, r.binding);
				continue;
			}
		}
	}

} // namespace BHive