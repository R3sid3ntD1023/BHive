#include "VulkanPipeline.h"
#include "VulkanShader.h"
#include "VulkanVertexArray.h"
#include "VulkanRendererAPI.h"
#include "VulkanBuffers.h"
#include "VulkanSwapChain.h"
#include "VulkanConverters.h"
#include "gfx/BufferBase.h"
#include "VulkanFramebuffer.h"
#include "core/Window.h"
#include "VulkanWindowContext.h"
#include "gfx/RenderCommand.h"
#include "IVulkanTexture.h"
#include "gfx/GlobalBuffers.h"
#include "VulkanUniformBuffer.h"

namespace BHive
{
	struct PendingDeletion
	{
		uint32_t Frame = 0;
		std::function<void(uint32_t)> Fn;
	};

	

	static std::queue<PendingDeletion> sDeletionQueue;

	VulkanRendererAPI::VulkanRendererAPI()
		: mDevice(VulkanBackend::GetLogicalDevice())
	{

	}

	VulkanRendererAPI::~VulkanRendererAPI()
	{
		
	}

	void VulkanRendererAPI::Init()
	{
		

		const uint32_t descriptor_count = 1000;

		std::vector<vk::DescriptorPoolSize> pool_sizes;
		pool_sizes.emplace_back(vk::DescriptorType::eSampler, descriptor_count);
		pool_sizes.emplace_back(vk::DescriptorType::eSampledImage, descriptor_count);
		pool_sizes.emplace_back(vk::DescriptorType::eCombinedImageSampler, descriptor_count);
		pool_sizes.emplace_back(vk::DescriptorType::eStorageImage, descriptor_count);
		pool_sizes.emplace_back(vk::DescriptorType::eUniformBuffer, descriptor_count);
		pool_sizes.emplace_back(vk::DescriptorType::eUniformBufferDynamic, descriptor_count);
		pool_sizes.emplace_back(vk::DescriptorType::eUniformTexelBuffer, descriptor_count);
		pool_sizes.emplace_back(vk::DescriptorType::eStorageBuffer, descriptor_count);
		pool_sizes.emplace_back(vk::DescriptorType::eStorageBufferDynamic, descriptor_count);
		pool_sizes.emplace_back(vk::DescriptorType::eStorageTexelBuffer, descriptor_count);
		pool_sizes.emplace_back(vk::DescriptorType::eInputAttachment, descriptor_count);

		vk::DescriptorPoolCreateInfo pool_create_info(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet | vk::DescriptorPoolCreateFlagBits::eUpdateAfterBind, 1000, pool_sizes);
		mDescriptorPool = mDevice.createDescriptorPool(pool_create_info);

	}

	void VulkanRendererAPI::Shutdown()
	{
		LOG_TRACE("RendererAPI Shutdown Called")

		mDescriptorPool = VK_NULL_HANDLE;

		VulkanBackend::Get().Shutdown();
	}

	void VulkanRendererAPI::WaitIdle()
	{
		LOG_TRACE("RendererAPI Wait Idle")
		VulkanBackend::GetLogicalDevice().waitIdle();
	}

	vk::Result VulkanRendererAPI::RenderFrame(VulkanWindowContext *ctx)
	{
		FResourceUpdateList mergedUpdates;
		for (auto &u : mSubmittedUpdates)
			mergedUpdates.Append(u);

		RenderGraph finalGraph;
		for (auto &g : mSubmittedGraphs)
			finalGraph.Append(g);

		mSubmittedGraphs.clear();
		mSubmittedUpdates.clear();

		if (finalGraph.Empty())
		{
		
		}

		return ExecuteFinalGraph(ctx, mergedUpdates, finalGraph);
	}

	void VulkanRendererAPI::SubmitGraph(const RenderGraph &graph, FResourceUpdateList &updateResources)
	{
		if (mDeviceRecreationInProgress.load())
			return;

		if (!graph.Empty())
			mSubmittedGraphs.emplace_back(graph);

		if (!updateResources.Empty())
			mSubmittedUpdates.push_back(updateResources);
	}

	void VulkanRendererAPI::SubmitResourceUpdate(FResourceUpdateList::UpdateCommand cmd)
	{
		if (mDeviceRecreationInProgress.load())
			return;

		FResourceUpdateList list{};
		list.Push(std::move(cmd));
		mSubmittedUpdates.push_back(list);
	}

	void VulkanRendererAPI::QueueDeletion(std::function<void(uint32_t)> fn)
	{
		sDeletionQueue.emplace(mCompletedFrame, fn);
	}

	void VulkanRendererAPI::UpdateGlobalSet(const VulkanShader &shader, const FSetReflection &set, uint32_t frame)
	{
		auto& device = VulkanBackend::GetLogicalDevice();

		uint64_t set_hash = shader.GetSetHashes().at(GLOBAL_SET_INDEX);
		if (!mGlobalSetSystem.Contains(set_hash))
		{
			auto manager = CreateScope<SetManager>(set, GLOBAL_SET_INDEX);
			manager->Init(device, mDescriptorPool, shader.GetDescriptorSetLayout(0));

			mGlobalSetSystem.Register(set_hash, manager);
		}

		auto manager = mGlobalSetSystem.Get(set_hash);
		manager->Update(frame, device);
	}

	vk::DescriptorSet VulkanRendererAPI::GetGlobalSet(uint64_t setHash, uint32_t frame) const
	{
		return mGlobalSetSystem.Get(setHash)->Get(frame);
	}

	void VulkanRendererAPI::ProcessDeletionQueue(uint32_t frame)
	{
		while (!sDeletionQueue.empty())
		{
			auto & del = sDeletionQueue.front();

			if (frame < del.Frame)
				del.Fn(frame);
			sDeletionQueue.pop();
		}
	}

	vk::Result VulkanRendererAPI::ExecuteFinalGraph(VulkanWindowContext *ctx, FResourceUpdateList &updates, const RenderGraph &graph)
	{
		auto current_frame = ctx->GetCurrentFrame();
		auto &cmd = ctx->GetCommandBuffer();

		ProcessDeletionQueue(current_frame);

		auto swap_chain = ctx->GetSwapChain();

		swap_chain->WaitForFence(current_frame);

		cmd.reset();

		auto [result, imageIndex] = swap_chain->AquireNextImage(current_frame);

		auto &image = swap_chain->GetImage(imageIndex);
		auto &depth = swap_chain->GetDepthImage();
		auto extent = swap_chain->GetExtent();

		if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
			return result;

		cmd.begin({});

		FVulkanRendererContext frame(cmd, current_frame, imageIndex);

		updates.Execute(frame);

		for (auto& pass : graph.GetPasses())
		{
			if (pass.Type == EPassType::SwapChain)
			{
				ImageState colorAttachmentState = {vk::ImageLayout::eColorAttachmentOptimal, vk::AccessFlagBits2::eColorAttachmentWrite, vk::PipelineStageFlagBits2::eColorAttachmentOutput};
				ImageState depthAttachmentState = {
					vk::ImageLayout::eDepthStencilAttachmentOptimal, vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
					vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests};

				image.Transition(frame.CommandBuffer, colorAttachmentState);
				depth.Transition(frame.CommandBuffer, depthAttachmentState);

				vk::ClearValue clearColor(mClearColor);
				vk::ClearValue clearDepth(vk::ClearDepthStencilValue(1.0f, 0));

				vk::RenderingAttachmentInfo attachmentInfo(
					image.GetView(), vk::ImageLayout::eColorAttachmentOptimal, {}, {}, vk::ImageLayout::eUndefined, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, clearColor);

				vk::RenderingAttachmentInfo depth_attachment_info(
					depth.GetView(), vk::ImageLayout::eDepthStencilAttachmentOptimal, {}, {}, vk::ImageLayout::eUndefined, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eDontCare, clearDepth);

				vk::RenderingInfo renderingInfo({}, vk::Rect2D({0, 0}, extent), 1, 0, attachmentInfo, &depth_attachment_info);
				frame.CommandBuffer.beginRendering(renderingInfo);

				pass.CommandList.Execute(frame);

				frame.CommandBuffer.endRendering();

				ImageState present = {vk::ImageLayout::ePresentSrcKHR, {}, vk::PipelineStageFlagBits2::eBottomOfPipe};
				image.Transition(frame.CommandBuffer, present);

			}
			else
			{
				pass.CommandList.Execute(frame);
			}
		}

		cmd.end();

		result = swap_chain->Present(cmd, imageIndex, current_frame);

		mCompletedFrame = current_frame;

		if (result != vk::Result::eSuccess)
			return result;

		return vk::Result::eSuccess;
	}

	void VulkanRendererAPI::SetCurrentContext(VulkanWindowContext *ctx)
	{
		mCurrentContext = ctx;
	}

	void VulkanRendererAPI::ClearColor(float r, float g, float b, float a)
	{
		mClearColor = {r, g, b, a};
	}

	void VulkanRendererAPI::Clear(ClearMask mask)
	{
		
	}

	void VulkanRendererAPI::SetLineWidth(float width)
	{
		auto &pass = RenderCommand::GetActivePass();
		pass.CommandList.Push(
			"Set Line Width",
			[=](IRendererContext &ctx)
			{
				auto &vk_ctx = CastRef<FVulkanRendererContext>(ctx);
				vk_ctx.CommandBuffer.setLineWidth(width);
			});
	}

	void VulkanRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h)
	{
		auto &pass = RenderCommand::GetActivePass();
		pass.CommandList.Push(
			"Set Viewport",
			[=](IRendererContext &ctx)
			{
				auto &vk_ctx = CastRef<FVulkanRendererContext>(ctx);
				vk_ctx.CommandBuffer.setViewport(0, vk::Viewport((float)x, (float)(y + h), (float)w, -(float)h, 0.0f, 1.0f));
				vk_ctx.CommandBuffer.setScissor(0, vk::Rect2D({(int32_t)x, (int32_t)y}, vk::Extent2D(w, h)));
			});
	}

	void VulkanRendererAPI::DrawArrays(ETopologyMode mode, const Ref<VertexArray> &vao, uint32_t count)
	{
		vao->Bind();

		auto topology = ToVkTopology(mode);

		auto &pass = RenderCommand::GetActivePass();
		pass.CommandList.Push(
			"Draw Arrays",
			[=](IRendererContext &ctx)
			{
				auto &vk_ctx = CastRef<FVulkanRendererContext>(ctx);
				vk_ctx.CommandBuffer.setPrimitiveTopology(topology);
				vk_ctx.CommandBuffer.draw(count, 1, 0, 0);
			});
	}

	void VulkanRendererAPI::DrawElements(ETopologyMode mode, const Ref<VertexArray> &vao, uint32_t count)
	{
		vao->Bind();
		auto index_buffer =vao->GetIndexBuffer();
		auto index_count = count ? count : index_buffer->GetCount();
		auto topology = ToVkTopology(mode);

		auto &pass = RenderCommand::GetActivePass();
		pass.CommandList.Push(
			"Draw Elements",
			[=](const IRendererContext &ctx)
			{
				auto &vk_ctx = static_cast<const FVulkanRendererContext &>(ctx);
				vk_ctx.CommandBuffer.setPrimitiveTopology(topology);
				vk_ctx.CommandBuffer.drawIndexed(index_count, 1, 0, 0, 0);
			});
	}

	void VulkanRendererAPI::DrawElementsBaseVertex(ETopologyMode mode, const VertexArray &vao, uint32_t start, uint32_t start_index, uint32_t count, uint32_t instance_count)
	{

	}

	void VulkanRendererAPI::DrawElementsRanged(ETopologyMode mode, const VertexArray &vao, uint32_t start, uint32_t end, uint32_t count)
	{
		
	}

	void VulkanRendererAPI::DrawElementsInstanced(ETopologyMode mode, const VertexArray &vao, uint32_t instances, uint32_t count)
	{
	
	}

	void VulkanRendererAPI::MultiDrawElementsIndirect(ETopologyMode mode, const BufferBase &indirect, const VertexArray &vao, size_t drawCount, size_t stride)
	{
		vao.Bind();

		auto buffer = indirect.GetNativeHandle().As<vk::Buffer>();
		auto topology = ToVkTopology(mode);

		auto &pass = RenderCommand::GetActivePass();
		pass.CommandList.Push("Multi Draw Elements Indirect", [buffer, topology, drawCount, stride](const IRendererContext &ctx)
		{		
			auto &vk_ctx = static_cast<const FVulkanRendererContext &>(ctx);
			vk_ctx.CommandBuffer.setPrimitiveTopology(topology);
			vk_ctx.CommandBuffer.drawIndexedIndirect(*buffer, 0, drawCount, stride);
		});

		vao.UnBind();
	}

	void VulkanRendererAPI::Dispath(uint32_t x, uint32_t y, uint32_t z)
	{
		auto &pass = RenderCommand::GetActivePass();
		pass.CommandList.Push("Set Viewport", [x, y, z](const IRendererContext &ctx)
			{
				auto& vk_ctx = static_cast<const FVulkanRendererContext &>(ctx);
				vk_ctx.CommandBuffer.dispatch(x, y, z);
		});

	}

	void VulkanRendererAPI::ColorMask(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
	{	
	}

	void VulkanRendererAPI::DebugPass(const std::string &msg)
	{
		LOG_TRACE(msg);
	}

} // namespace BHive
