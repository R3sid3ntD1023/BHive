#include "VulkanPipeline.h"
#include "VulkanConversions.h"
#include "gfx/RenderCommand.h"

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

	Ref<FVulkanPipelineConfigInfo> Convert(const Pipeline::GraphicsPipelineState &state)
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

	void VulkanPipeline::Init(const PipelineState *state)
	{
		mState = state->Clone();
	}

	void VulkanPipeline::Bind(vk::CommandBuffer cmd, uint32_t frame, uint32_t numAttachments)
	{
		// bind states
		if (mState->GetType() == Pipeline::PipelineState::Graphics)
		{
			auto state = Cast<GraphicsPipelineState>(mState.get());

			// raster
			cmd.setCullModeEXT(ToVkCull(state->Raster.CullMode));
			cmd.setFrontFaceEXT(ToVkFrontFace(state->Raster.FrontFace));
			cmd.setPolygonModeEXT(ToVkPolygon(state->Raster.FillMode));

			// depth
			cmd.setDepthTestEnableEXT(state->Depth.DepthTest);
			cmd.setDepthWriteEnableEXT(state->Depth.DepthWrite);
			cmd.setDepthCompareOpEXT(ToVkCompare(state->Depth.DepthCompare));
			cmd.setStencilTestEnable(VK_TRUE);
			cmd.setStencilOp(vk::StencilFaceFlagBits::eFrontAndBack, vk::StencilOp::eKeep, vk::StencilOp::eKeep, vk::StencilOp::eKeep, vk::CompareOp::eAlways);
			cmd.setStencilCompareMask(vk::StencilFaceFlagBits::eFrontAndBack, 0xFFFFFFFF);
			cmd.setStencilWriteMask(vk::StencilFaceFlagBits::eFrontAndBack, 0xFFFFFFFF);
			cmd.setStencilReference(vk::StencilFaceFlagBits::eFrontAndBack, 0);

			cmd.setDepthBiasEnable(VK_FALSE);

			cmd.setRasterizerDiscardEnable(VK_FALSE);
			cmd.setAlphaToCoverageEnableEXT(VK_TRUE);

			// Blend
			std::vector<vk::ColorBlendEquationEXT> equations(numAttachments);

			for (uint32_t i = 0; i < numAttachments; i++)
			{
				cmd.setColorWriteMaskEXT(i, vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
				cmd.setColorBlendEnableEXT(i, state->Blend.Enabled);

				vk::ColorBlendEquationEXT equation;
				equation.setSrcColorBlendFactor(ToVkBlendFactor(state->Blend.SrcColor));
				equation.setDstColorBlendFactor(ToVkBlendFactor(state->Blend.DstColor));
				equation.setColorBlendOp(ToVkBlendOp(state->Blend.ColorOp));
				equation.setSrcAlphaBlendFactor(ToVkBlendFactor(state->Blend.SrcAlpha));
				equation.setDstAlphaBlendFactor(ToVkBlendFactor(state->Blend.DstAlpha));
				equation.setAlphaBlendOp(ToVkBlendOp(state->Blend.AlphaOp));
				equations[i] = equation;
			}

			cmd.setColorBlendEquationEXT(0, equations);

			// topology
			// cmd.setPrimitiveTopologyEXT(ToVkTopology(state->DrawMode));
			cmd.setPrimitiveRestartEnable(VK_FALSE);

			// mulitsample
			cmd.setRasterizationSamplesEXT(vk::SampleCountFlagBits::e1);

			uint32_t samplemask = 0xFFFFFFFF;
			cmd.setSampleMaskEXT(vk::SampleCountFlagBits::e1, &samplemask);
		}
	}

} // namespace BHive