#include "VulkanCommandTranslater.h"
#include "gfx/rendergraph/Commands.h"
#include <backends/imgui_impl_vulkan.h>
#include "VulkanConversions.h"
#include "VulkanPipeline.h"
#include "VulkanShader.h"
#include "VulkanVertexArray.h"
#include "VulkanBackendMaterial.h"
#include "VulkanBuffers.h"
#include "gfx/material/Material.h"
#include "gfx/renderers/Renderer.h"
#include "gfx/Framebuffer.h"

namespace BHive
{
	template <typename T>
	const T &CmdCast(FCommand &cmd)
	{
		return static_cast<const T &>(cmd);
	}

	void VulkanCommandTranslator::ExecuteCommandList(const FPass &pass, const FPhase &phase, FVulkanRendererContext &ctx)
	{
		auto &cmdbuffer = ctx.CommandBuffer;
		const auto &frame = ctx.Frame;
		const auto &list = phase.CommandList;
		const auto numAttachments = phase.FBO ? phase.FBO->GetNumColorAttachments() : 0;

		for (auto &cmd : list.Commands)
		{
			switch (cmd->GetType())
			{
			case ECommandType::GenerateMipMaps:
			{
				auto &c = CmdCast<CmdGenerateMipMaps>(*cmd);
				auto vkImage = c.TextureRef->GetNativeHandle().As<VulkanImage>();
				vkImage->GenerateMipMaps(cmdbuffer);
			}
			break;
			case ECommandType::Dispatch:
			{
				auto &c = CmdCast<CmdDisptach>(*cmd);
				cmdbuffer.dispatch(c.X, c.Y, c.Z);
			}
			break;
			case ECommandType::ImGuiRender:
			{
				auto &c = CmdCast<CmdImGuiRender>(*cmd);
				ImGui_ImplVulkan_RenderDrawData(c.DrawData, *cmdbuffer);
			}
			break;
			case ECommandType::BindPipeline:
			{
				auto &c = CmdCast<CmdBindPipeline>(*cmd);
				auto pipeline = Cast<VulkanPipeline>(c.PipelineRef);
				pipeline->Bind(cmdbuffer, frame, numAttachments);
			}
			break;
			case ECommandType::BindMaterial:
			{
				auto &c = CmdCast<CmdBindMaterial>(*cmd);
				BindMaterialSnapshot(c.Snapshot, ctx, pass);
			}
			break;
			case ECommandType::UploadBuffer:
			{
				auto &c = CmdCast<CmdUploadBuffer>(*cmd);
				auto data = c.Data->data();
				auto size = c.Data->size();
				auto offset = c.Offset;

				c.Buffer->SetData(data, size, offset);
			}
			break;
			case ECommandType::DrawFullScreen:
			{
				cmdbuffer.setVertexInputEXT({}, {});
				cmdbuffer.setPrimitiveTopology(vk::PrimitiveTopology::eTriangleList);
				cmdbuffer.draw(3, 1, 0, 0);
			}
			break;
			case ECommandType::Draw:
			{
				auto &c = CmdCast<CmdDraw>(*cmd);
				auto vao = Cast<VulkanVertexArray>(c.VAO);

				vao->Bind(cmdbuffer, frame);

				cmdbuffer.setPrimitiveTopology(ToVkTopology(c.Mode));
				cmdbuffer.draw(c.Count, 1, 0, 0);
			}
			break;
			case ECommandType::DrawIndexed:
			{
				auto &c = CmdCast<CmdDrawIndexed>(*cmd);
				auto vao = Cast<VulkanVertexArray>(c.VAO);
				auto indexBuffer = c.VAO->GetIndexBuffer();
				auto count = c.Count ? c.Count : indexBuffer->GetCount();
				auto topology = ToVkTopology(c.Mode);

				vao->Bind(cmdbuffer, frame);
				cmdbuffer.setPrimitiveTopology(topology);
				cmdbuffer.drawIndexed(count, 1, 0, 0, 0);
			}
			break;
			case ECommandType::MultiDrawIndexedIndirect:
			{
				auto &c = CmdCast<CmdMultiDrawIndexedIndirect>(*cmd);
				auto topology = ToVkTopology(c.Mode);
				auto handle = c.Buffer->GetNativeHandle().As<VulkanBuffer>();
				auto &buf = handle->GetNative(frame);
				auto vao = Cast<VulkanVertexArray>(c.VAO);
				auto stride = (uint32_t)c.Stride;

				vao->Bind(cmdbuffer, frame);
				cmdbuffer.setPrimitiveTopology(topology);
				cmdbuffer.drawIndexedIndirect(buf.GetBuffer(), c.Offset * stride, c.DrawCount, stride);
			}
			break;
			case ECommandType::SetLineWidth:
			{
				auto &c = CmdCast<CmdSetLineWidth>(*cmd);
				cmdbuffer.setLineWidth(c.Width);
			}
			break;
			default:
				break;
			}
		}
	}

	void VulkanCommandTranslator::CreateBarriers(const FRenderCommandList &list, FVulkanRendererContext &ctx)
	{
		if (list.BufferBarriers.empty())
			return;

		auto &cmd = ctx.CommandBuffer;
		auto frame = ctx.Frame;

		std::vector<vk::BufferMemoryBarrier2> bufBarriers;
		bufBarriers.reserve(list.BufferBarriers.size());

		for (auto &b : list.BufferBarriers)
		{
			auto handle = b.Buffer->GetNativeHandle().As<VulkanBuffer>();
			vk::Buffer buf = handle->GetNative(frame).GetBuffer();

			bufBarriers.emplace_back(ToStage(b.Src), ToAccess(b.Src), ToStage(b.Dst), ToAccess(b.Dst), VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, buf, 0, VK_WHOLE_SIZE);

			vk::DependencyInfo depInfo({}, {}, bufBarriers);
			cmd.pipelineBarrier2(depInfo);
		}
	}

	void VulkanCommandTranslator::BindGlobals(VulkanShader *shader, const FPass &pass)
	{
		for (auto &[binding, buffer] : pass.GlobalBuffers)
			shader->BindGlobal(binding.Set, binding.Binding, buffer);

		for (auto &[binding, texture] : pass.GlobalTextures)
			shader->BindGlobal(binding.Set, binding.Binding, texture);
	}

	void VulkanCommandTranslator::BindMaterialSnapshot(const MaterialSnapshot &snap, FVulkanRendererContext &ctx, const FPass &pass)
	{
		auto &cmd = ctx.CommandBuffer;
		const auto frame = ctx.Frame;

		auto shader = Cast<VulkanShader>(snap.Shader);
		if (!shader)
			return;

		BindGlobals(shader.get(), pass);

		if (auto materialGroup = shader->GetBindingGroup(MATERIAL_SET_INDEX))
		{
			BindMaterialResources(snap, *materialGroup);
		}

		if (auto batchGroup = shader->GetBindingGroup(BATCH_SET_INDEX))
		{
			BindObjectResources(ctx.ModelBuffer, *batchGroup);
		}

		if (auto passGroup = shader->GetBindingGroup(PASS_SET_INDEX))
		{
			BindMaterialResources(snap, *passGroup);
		}

		shader->Bind(cmd, frame);

		for (auto &pc : snap.mReflection->PushConstants)
		{
			shader->BindPushConstants(cmd, ToVkShaderStageBit(pc.Stages), snap.PushConstantData.data() + pc.Offset, (uint32_t)pc.Size, pc.Offset);
		}
	}

	void VulkanCommandTranslator::BindMaterialResources(const MaterialSnapshot &snap, VulkanBindingGroup &group)
	{
		for (auto &[name, tb] : snap.Textures)
		{
			group.SetTexture(tb.Binding, tb.TextureRef, tb.BaseMipLevel);
		}

		for (auto &[name, buf] : snap.LocalBuffers)
		{
			group.SetBuffer(buf.Binding, buf.BufferRef);
		}
	}

	void VulkanCommandTranslator::BindObjectResources(const Ref<GeneralBuffer> &buffer, VulkanBindingGroup &group)
	{
		// group.SetBuffer(0, buffer);
	}

} // namespace BHive