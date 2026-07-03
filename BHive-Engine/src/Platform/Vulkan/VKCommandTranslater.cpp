#include "VKCommandTranslater.h"
#include "gfx/rendergraph/Commands.h"
#include <backends/imgui_impl_vulkan.h>
#include "VulkanConversions.h"
#include "VulkanPipeline.h"
#include "VulkanVertexArray.h"
#include "VulkanBackendMaterial.h"
#include "VulkanBuffers.h"
#include "gfx/material/Material.h"

namespace BHive
{
	template<typename T>
	const T& CmdCast(FCommand& cmd)
	{
		return static_cast<const T &>(cmd);
	}

	void VKCommandTranslator::ExecuteCommandList(const FRenderCommandList &list, FVulkanRendererContext &ctx)
	{
		auto &cmdbuffer = ctx.CommandBuffer;
		const auto frame = ctx.Frame;

		for (auto& cmd : list.Commands)
		{
			switch (cmd->GetType())
			{
			case ECommandType::SetClearColor:
			{
				auto &c = CmdCast<CmdSetClearColor>(*cmd);
				mClearColor = vk::ClearColorValue(c.R, c.G, c.B, c.A);
			}
			break;
			case ECommandType::SetViewport:
			{
				auto &c = CmdCast<CmdSetViewport>(*cmd);
				cmdbuffer.setViewport(0, vk::Viewport(float(c.X), float(c.Y + c.Height), float(c.Width), - float(c.Height), 0.0f, 1.0f));
				cmdbuffer.setScissor(0, vk::Rect2D({c.X, c.Y}, {c.Width, c.Height}));
			}
			break;
			case ECommandType::GenerateMipMaps:
			{
				auto &c = CmdCast<CmdGenerateMipMaps>(*cmd);
				auto vkImage = c.Tex->GetNativeHandle().As<VulkanImage>();
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
				pipeline->Bind(cmdbuffer, frame);
			}
			break;
			case ECommandType::BindMaterial:
			{
				auto &c = CmdCast<CmdBindMaterial>(*cmd);
				auto vkPipeline = Cast<VulkanPipeline>(c.PipelineRef);
				BindMaterialSnapshot(c.Snapshot, vkPipeline, ctx);
			}
			break;
			case ECommandType::UploadBuffer:
			{
				auto &c = CmdCast<CmdUploadBuffer>(*cmd);
				auto native = c.Buffer->GetNativeHandle();
				auto data = c.Data->data();
				auto size = c.Data->size();
				auto offset = c.Offset;

				FBufferUploadInfo upload
				{	
					.data = data, 
					.size = size, 
					.offset = offset
				};

				if (auto b = native.As<VulkanStaticBuffer>())
				{
					b->Upload(cmdbuffer, upload);
				}
				else if (auto b = native.As<VulkanPerFrameHostBuffer>())
				{
					//for(uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
					b->Upload(frame, upload);
				}
				else
				{
					ASSERT(false, "Unknown buffer type in CmdUploadBuffer");
				}
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
				auto buffer = c.Buffer->GetNativeHandle().As<AllocatedBuffer>()->GetBuffer();
				auto vao = Cast<VulkanVertexArray>(c.VAO);

				vao->Bind(cmdbuffer, frame);
				cmdbuffer.setPrimitiveTopology(topology);
				cmdbuffer.drawIndexedIndirect(buffer, c.Offset, c.DrawCount, (uint32_t)c.Stride);
			}
			break;
			case ECommandType::SetGlobalTopology:
			{
				auto &c = CmdCast<CmdSetGlobalTopology>(*cmd);
				mGlobalTopology = ToVkTopology(c.Mode);
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

	void VKCommandTranslator::CreateBarriers(const FRenderCommandList &list, FVulkanRendererContext& ctx)
	{
		if (list.BufferBarriers.empty())
			return;

		auto& cmd = ctx.CommandBuffer;
		auto frame = ctx.Frame;

		std::vector<vk::BufferMemoryBarrier2> bufBarriers;
		bufBarriers.reserve(list.BufferBarriers.size());

		for (auto& b : list.BufferBarriers)
		{
			auto handle = b.Buffer->GetNativeHandle(frame);

			vk::Buffer bufferVk;
			if (handle.As<VulkanStaticBuffer>())
				bufferVk = handle.As<VulkanStaticBuffer>()->Buffer.GetBuffer();
			else if (handle.As<VulkanPerFrameHostBuffer>())
				bufferVk = handle.As<VulkanPerFrameHostBuffer>()->GetBuffer(frame);
			else
			{
				ASSERT(false && "Unknown buffer type");
			}
			bufBarriers.emplace_back(
				ToStage(b.Src), 
				ToAccess(b.Src), 
				ToStage(b.Dst), 
				ToAccess(b.Dst), 
				VK_QUEUE_FAMILY_IGNORED, 
				VK_QUEUE_FAMILY_IGNORED, 
				bufferVk, 
				0, 
				VK_WHOLE_SIZE
			);

			vk::DependencyInfo depInfo({}, {}, bufBarriers);
			cmd.pipelineBarrier2(depInfo);
		}
	}

	void VKCommandTranslator::BindMaterialSnapshot(const MaterialSnapshot &snap, VulkanPipeline *pipeline, FVulkanRendererContext &ctx)
	{
		auto &cmd = ctx.CommandBuffer;
		const auto frame = ctx.Frame;
		auto refl = pipeline->GetShaderProgram()->GetRefl();
		auto mergedRefl = pipeline->GetShaderProgram()->GetMergedRefl();

		pipeline->Bind(cmd, frame);

		if (!pipeline->HasSet(MATERIAL_SET_INDEX))
			return;

		auto group = Cast<VulkanBindingGroup>(pipeline->GetOrCreateBindingGroup(MATERIAL_SET_INDEX));

		auto &setBindings = refl.GetSetBindings(MATERIAL_SET_INDEX);

		for (auto &[name ,tb] : snap.Textures)
		{
			group->SetTexture(tb.Binding, tb.Texture, tb.Mip);
		}

		for (auto &[name, buf] : snap.LocalBuffers)
		{
			group->SetBuffer(refl.FindByName(name)->binding, buf);
		}

		

		auto matSet = group->GetOrCreateMaterialSet();
		cmd.bindDescriptorSets(pipeline->GetBindPoint(), pipeline->GetLayout(), MATERIAL_SET_INDEX, matSet, {});

		for (auto &pc : mergedRefl.PushConstants)
		{
			vk::PushConstantsInfo info(pipeline->GetLayout(), ToVkShaderStageBit(pc.Stages), pc.Offset, (uint32_t)pc.Size, snap.PushConstantData.data() + pc.Offset);
			cmd.pushConstants2(info);
		}
		
	}
} // namespace BHive