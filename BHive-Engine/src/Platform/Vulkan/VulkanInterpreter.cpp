#include "VulkanInterpreter.h"
#include "VulkanBackendMaterial.h"
#include "VulkanBuffers.h"
#include "VulkanConversions.h"
#include "VulkanPipeline.h"
#include "VulkanShader.h"
#include "VulkanVertexArray.h"
#include "gfx/Framebuffer.h"
#include "gfx/material/Material.h"
#include "gfx/renderers/Renderer.h"
#include "gfx/rendergraph/Commands.h"
#include <backends/imgui_impl_vulkan.h>

namespace BHive
{
	void VulkanInterpreter::ExecuteCommandList(const FPhase &phase, FVulkanRendererContext &ctx)
	{
		auto &cmdbuffer = ctx.CommandBuffer;
		const auto &frame = ctx.Frame;
		const auto numAttachments = phase.FBO ? phase.FBO.As<Framebuffer>()->GetNumColorAttachments() : 0;

		auto it = phase.Commands.begin();
		auto end = phase.Commands.end();

		while (it != end)
		{
			const CmdHeader *header = reinterpret_cast<const CmdHeader *>(&(*it).first);

			const auto *payloadPtr = (*it).second.get();

			switch (header->Type)
			{
			case ECommandType::SetBufferData:
			{
				auto &c = *reinterpret_cast<const CmdSetBufferData *>(payloadPtr);
				auto buffer = c.Buffer.As<BufferBase>()->GetNativeHandle().As<VulkanBuffer>();
				buffer->Upload(c.Data.data(), c.Size, c.Offset);
			}
			break;
			case ECommandType::ClearBuffer:
			{
				auto &c = *reinterpret_cast<const CmdClearBuffer *>(payloadPtr);
				auto buffer = c.Buffer.As<BufferBase>()->GetNativeHandle().As<VulkanBuffer>();
				buffer->ClearData();
			}
			break;
			case ECommandType::GenerateMipMaps:
			{
				auto &c = *reinterpret_cast<const CmdGenerateMipMaps *>(payloadPtr);
				auto vkImage = c.Texture.As<Texture>()->GetNativeHandle().As<VulkanImage>();
				vkImage->GenerateMipMaps(cmdbuffer);
			}
			break;
			case ECommandType::Dispatch:
			{
				auto &c = *reinterpret_cast<const CmdDispatch *>(payloadPtr);
				;
				cmdbuffer.dispatch(c.X, c.Y, c.Z);
			}
			break;
			case ECommandType::ImGuiRender:
			{
				auto &c = *reinterpret_cast<const CmdImGuiRender *>(payloadPtr);
				ImGui_ImplVulkan_RenderDrawData(c.DrawData, *cmdbuffer);
			}
			break;
			case ECommandType::BindPipeline:
			{
				auto &c = *reinterpret_cast<const CmdBindPipeline *>(payloadPtr);
				auto pipeline = c.Pipeline.As<VulkanPipeline>();
				pipeline->Bind(cmdbuffer, frame, numAttachments);
			}
			break;
			case ECommandType::BindMaterial:
			{
				auto &c = *reinterpret_cast<const CmdBindMaterial *>(payloadPtr);
				BindMaterialSnapshot(c.Snapshot, ctx, phase);
			}
			break;
			case ECommandType::UploadBuffer:
			{
				auto &c = *reinterpret_cast<const CmdUploadBuffer *>(payloadPtr);
				auto data = c.Data->data();
				auto size = c.Data->size();
				auto offset = c.Offset;

				c.Buffer.As<BufferBase>()->SetData(data, size, offset);
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
				auto &c = *reinterpret_cast<const CmdDraw *>(payloadPtr);

				if (c.VAO)
				{
					c.VAO.As<VulkanVertexArray>()->Bind(cmdbuffer, frame);
				}
				else
				{
					cmdbuffer.setVertexInputEXT({}, {});
				}

				cmdbuffer.setPrimitiveTopology(ToVkTopology(c.Mode));
				cmdbuffer.draw(c.Count, 1, 0, 0);
			}
			break;
			case ECommandType::DrawIndexed:
			{
				auto &c = *reinterpret_cast<const CmdDrawIndexed *>(payloadPtr);
				if (!c.VAO)
					break;

				auto vao = c.VAO.As<VulkanVertexArray>();
				vao->Bind(cmdbuffer, frame);

				auto indexBuffer = vao->GetIndexBuffer().As<IndexBuffer>();
				auto count = c.Count ? c.Count : indexBuffer->GetCount();

				auto topology = ToVkTopology(c.Mode);
				cmdbuffer.setPrimitiveTopology(topology);
				cmdbuffer.drawIndexed(count, 1, 0, 0, 0);
			}
			break;
			case ECommandType::MultiDrawIndexedIndirect:
			{
				auto &c = *reinterpret_cast<const CmdMultiDrawIndexedIndirect *>(payloadPtr);
				auto topology = ToVkTopology(c.Mode);
				auto handle = c.Buffer.As<BufferBase>()->GetNativeHandle().As<VulkanBuffer>();
				vk::Buffer buf = handle->GetNative(frame)->Buffer;

				auto vao = c.VAO.As<VulkanVertexArray>();
				auto stride = (uint32_t)c.Stride;

				vao->Bind(cmdbuffer, frame);
				cmdbuffer.setPrimitiveTopology(topology);
				cmdbuffer.drawIndexedIndirect(buf, c.Offset, c.DrawCount, stride);
			}
			break;
			case ECommandType::SetLineWidth:
			{
				auto &c = *reinterpret_cast<const CmdSetLineWidth *>(payloadPtr);
				cmdbuffer.setLineWidth(c.Width);
			}
			break;
			default:
				break;
			}

			++it;
		}
	}

	void VulkanInterpreter::CreateBarriers(const std::vector<FBufferTransition> &transitions, FVulkanRendererContext &ctx)
	{
		if (transitions.empty())
			return;

		auto &cmd = ctx.CommandBuffer;
		auto frame = ctx.Frame;

		std::vector<vk::BufferMemoryBarrier2> bufBarriers;
		bufBarriers.reserve(transitions.size());

		for (auto &t : transitions)
		{
			auto handle = t.Buffer.As<BufferBase>()->GetNativeHandle().As<VulkanBuffer>();
			vk::Buffer buf = handle->GetNative(frame)->Buffer;

			auto srcStage = ToStage(t.Src);
			auto dstStage = ToStage(t.Dst);
			auto srcAccess = ToAccess(t.Src);
			auto dstAccess = ToAccess(t.Dst);

			bufBarriers.emplace_back(srcStage, srcAccess, dstStage, dstAccess, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, buf, 0, VK_WHOLE_SIZE);

			vk::DependencyInfo depInfo({}, {}, bufBarriers);
			cmd.pipelineBarrier2(depInfo);
		}
	}

	void VulkanInterpreter::BindMaterialSnapshot(const MaterialSnapshot &snap, FVulkanRendererContext &ctx, const FPhase &phase)
	{
		auto &cmd = ctx.CommandBuffer;
		const auto frame = ctx.Frame;

		// bind globals
		for (auto &group : snap.BindingGroups)
		{
			auto set = group->GetSetIndex();
			for (auto &[binding, buffer] : phase.BoundBuffers)
			{
				if (binding.Set == set)
				{
					group->SetBuffer(binding.Binding, buffer);
				}
			}

			for (auto &[binding, texture] : phase.BoundTextures)
			{
				if (binding.Set == set)
				{
					group->SetTexture(binding.Binding, texture);
				}
			}
		}

		for (auto &group : snap.BindingGroups)
		{
			auto set = group->GetSetIndex();
			if (set != MATERIAL_SET_INDEX)
				continue;

			for (auto &[binding, tb] : snap.Textures)
			{
				group->SetTexture(binding, tb.Texture, tb.BaseMipLevel);
			}

			for (auto &[binding, buf] : snap.Buffers)
			{
				group->SetBuffer(binding, buf.Buffer);
			}
		}

		auto shader = snap.Shader.As<VulkanShader>();
		// if (CurrentBoundShader.As<VulkanShader>() != shader)
		// {
		// 	CurrentBoundShader = snap.Shader;
		// 	shader = CurrentBoundShader.As<VulkanShader>();
		// }

		shader->Bind(cmd);

		auto &shaderTemplate = shader->GetTemplate();

		for (auto &group : snap.BindingGroups)
		{
			shader->BindGroup(cmd, frame, group.get());
		}

		for (auto &pc : shaderTemplate.PushConstants)
		{
			shader->BindPushConstants(cmd, ToVkShaderStageBit(pc.Stages), snap.PushConstantData.data() + pc.Offset, (uint32_t)pc.Size, pc.Offset);
		}
	}

} // namespace BHive