#include "VulkanPipeline.h"
#include "VulkanShader.h"
#include "VulkanVertexArray.h"
#include "VulkanRendererAPI.h"
#include "VulkanSwapChain.h"
#include "VulkanConverters.h"
#include "gfx/BufferBase.h"
#include "VulkanFramebuffer.h"
#include "VulkanWindowContext.h"
#include "gfx/RenderCommand.h"
#include "VulkanSetManager.h"
#include "systems/GlobalSetRegistry.h"
#include "systems/MaterialSetRegistry.h"
#include "textures/VulkanImage.h"
#include "pass/ComputeBindings.h"

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

		AddSubSystem<GlobalSetRegistry>();
		AddSubSystem<MaterialSetRegistry>();
	}

	void VulkanRendererAPI::Shutdown()
	{
		LOG_TRACE("RendererAPI Shutdown Called")

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
			return vk::Result::eSuccess;
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


	Ref<ISetManager> VulkanRendererAPI::CreateSetManager(const Pipeline* pipeline, uint32_t setIndex)
	{
		auto program = pipeline->GetShaderProgram();
		auto& refl = program->GetRefl();
		auto vKpipeline = Cast<VulkanPipeline>(pipeline);
		auto layout = vKpipeline->GetSetLayout(setIndex);

		auto manager = CreateRef<VulkanSetManager>(VulkanBackend::GetLogicalDevice(), mDescriptorPool, layout, setIndex,
			refl);
		return manager;
	}

	void VulkanRendererAPI::OnPipelineCreated(const VulkanPipeline *pipeline)
	{
		auto &registry = GetSubSystem<GlobalSetRegistry>();
		registry.EnsureGlobalSet(*pipeline, GLOBAL_SET_INDEX);
	}

	void VulkanRendererAPI::ProcessDeletionQueue(uint32_t frame)
	{
		while (!sDeletionQueue.empty())
		{
			auto & del = sDeletionQueue.front();

			if (frame > del.Frame)
				del.Fn(frame);
			sDeletionQueue.pop();
		}
	}

	vk::Result VulkanRendererAPI::ExecuteFinalGraph(VulkanWindowContext *ctx, FResourceUpdateList &updates, const RenderGraph &graph)
	{
		auto current_frame = ctx->GetCurrentFrame();
		auto &cmd = ctx->GetCommandBuffer();

		auto& swap_chain = ctx->GetSwapChain();
		swap_chain->WaitForFence(current_frame);

		ProcessDeletionQueue(current_frame);

		cmd.reset();

		auto [result, imageIndex] = swap_chain->AquireNextImage(current_frame);

		if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
			return result;

		cmd.begin({});

		vk::DebugUtilsLabelEXT label_info("Main Pass", std::array<float, 4>{0.0f, 1.0f, 0.0f, 1.0f});
		cmd.beginDebugUtilsLabelEXT(label_info);

		FVulkanRendererContext vk_ctx(cmd, current_frame, imageIndex);

		GetSubSystem<GlobalSetRegistry>().UpdatePerFrame(current_frame);
		GetSubSystem<MaterialSetRegistry>().UpdatePerFrame(current_frame);

		updates.Execute(vk_ctx);

		for (auto& pass : graph.GetPasses())
		{
			vk::DebugUtilsLabelEXT debugInfo(pass.Name.c_str(), {1, 0, 0, 1});

			cmd.beginDebugUtilsLabelEXT(debugInfo);

			if (pass.Type == EPassType::SwapChain)
			{
				ExecuteSwapChainPass(pass, vk_ctx, swap_chain);
			}
			else if (pass.Type == EPassType::OffScreen)
			{
				ExecuteOffScreenPass(pass, vk_ctx);
			}
			else if (pass.Type == EPassType::Transfer)
			{
				for (auto &image : pass.Images)
				{
					ImageSubresource sub{};
					sub.MipLevel = image.BaseMip;
					sub.LevelCount = image.LevelCount;
					sub.BaseArrayLayer = image.BaseLayer;
					sub.LayerCount = image.LayerCount;

					auto img = image.Texture->GetNativeHandle().As<VulkanImage>();
					img->Transition(cmd, ImageState::TansferWrite(), sub);
				}
				
				pass.CommandList.Execute(vk_ctx);

				for (auto &image : pass.Images)
				{
					ImageSubresource sub{};
					sub.MipLevel = image.BaseMip;
					sub.LevelCount = image.LevelCount;
					sub.BaseArrayLayer = image.BaseLayer;
					sub.LayerCount = image.LayerCount;

					auto img = image.Texture->GetNativeHandle().As<VulkanImage>();
					img->Transition(cmd, ImageState::ShaderRead(), sub);
				}
			}

			cmd.endDebugUtilsLabelEXT();
		}

		cmd.endDebugUtilsLabelEXT();
		cmd.end();

		result = swap_chain->Present(cmd, imageIndex, current_frame);

		mCompletedFrame = current_frame;

		return result;
	}

	void VulkanRendererAPI::ExecuteSwapChainPass(const FRenderGraphPass &pass, FVulkanRendererContext &ctx, const Ref<VulkanSwapChain> &swapChain)
	{
		auto &image = swapChain->GetImage(ctx.ImageIndex);
		auto &depth = swapChain->GetDepthImage();
		auto extent = swapChain->GetExtent();
		auto &cmd = ctx.CommandBuffer;

		// Color: Undefined/ShaderRead/etc → ColorAttachment
		image.Transition(cmd, ImageState::ColorAttachment());

		// Depth: Undefined/ShaderRead/etc → DepthStencilAttachment
		depth.Transition(cmd, ImageState::DepthStencilAttachment());

		vk::ClearValue clearColor(mClearColor);
		vk::ClearValue clearDepth(vk::ClearDepthStencilValue(1.0f, 0));

		vk::RenderingAttachmentInfo attachmentInfo(
			image.Native().GetView(0, 0, 0), vk::ImageLayout::eColorAttachmentOptimal, {}, {}, vk::ImageLayout::eUndefined, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, clearColor);

		vk::RenderingAttachmentInfo depth_attachment_info(
			depth.Native().GetView(0, 0, 0), vk::ImageLayout::eDepthStencilAttachmentOptimal, {}, {}, vk::ImageLayout::eUndefined, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eDontCare,
			clearDepth);

		vk::RenderingInfo renderingInfo({}, vk::Rect2D({0, 0}, extent), 1, 0, attachmentInfo, &depth_attachment_info);
		cmd.beginRendering(renderingInfo);

		pass.CommandList.Execute(ctx);

		cmd.endRendering();

		image.Transition(cmd, ImageState::Present());
	}

	void VulkanRendererAPI::ExecuteOffScreenPass(const FRenderGraphPass &pass, IRendererContext &ctx)
	{
		pass.CommandList.Execute(ctx);
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

	void VulkanRendererAPI::DrawElementsBaseVertex(ETopologyMode mode, const Ref<VertexArray> &vao, uint32_t start, uint32_t start_index, uint32_t count, uint32_t instance_count)
	{
		vao->Bind();
		auto index_buffer = vao->GetIndexBuffer();
		auto index_count = count ? count : index_buffer->GetCount();
		auto topology = ToVkTopology(mode);

		auto &pass = RenderCommand::GetActivePass();
		pass.CommandList.Push(
			"Draw Elements",
			[=](const IRendererContext &ctx)
			{
				auto &vk_ctx = static_cast<const FVulkanRendererContext &>(ctx);
				vk_ctx.CommandBuffer.setPrimitiveTopology(topology);
				vk_ctx.CommandBuffer.drawIndexed(index_count, instance_count, start_index, start, 0);
			});
	}

	void VulkanRendererAPI::DrawElementsRanged(ETopologyMode mode, const Ref<VertexArray> &vao, uint32_t start, uint32_t end, uint32_t count)
	{
		
	}

	void VulkanRendererAPI::DrawElementsInstanced(ETopologyMode mode, const Ref<VertexArray> &vao, uint32_t instances, uint32_t count)
	{
	
	}

	void VulkanRendererAPI::MultiDrawElementsIndirect(ETopologyMode mode, const BufferBase &indirect, const Ref<VertexArray> &vao, size_t drawCount, size_t stride)
	{
		vao->Bind();

		auto buffer = indirect.GetNativeHandle().As<AllocatedBuffer>()->GetBuffer();
		auto topology = ToVkTopology(mode);

		auto &pass = RenderCommand::GetActivePass();
		pass.CommandList.Push("Multi Draw Elements Indirect", [buffer, topology, drawCount, stride](const IRendererContext &ctx)
		{		
			auto &vk_ctx = static_cast<const FVulkanRendererContext &>(ctx);
			vk_ctx.CommandBuffer.setPrimitiveTopology(topology);
			vk_ctx.CommandBuffer.drawIndexedIndirect(buffer, 0, drawCount, stride);
		});

		vao->UnBind();
	}

	void VulkanRendererAPI::ColorMask(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
	{	
	}

	void VulkanRendererAPI::DebugPass(const std::string &msg)
	{
		LOG_TRACE(msg);
	}

	Ref<FComputeBindings> VulkanRendererAPI::CreateComputeBindings(const Ref<Pipeline> &pipeline)
	{
		auto vkPipeline = Cast<VulkanPipeline>(pipeline);
		return CreateRef<FVulkanComputeBindings>(vkPipeline);
	}

	AsyncComputeHandle VulkanRendererAPI::ExecuteComputePass(const Ref<Pipeline> &pipeline, const glm::uvec3 & size, const FComputeFunc &builder)
	{
		vk::CommandBufferAllocateInfo allocInfo(VulkanBackend::GetImmediateCommandPool(), vk::CommandBufferLevel::ePrimary, 1);

		auto cmds = (*mDevice).allocateCommandBuffers(allocInfo);
		auto& cmd = cmds[0];

		vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

		cmd.begin(beginInfo);

		vk::DebugUtilsLabelEXT labelInfo("Compute Pass", {1, .5, 0, 1 });
		cmd.beginDebugUtilsLabelEXT(labelInfo);

		auto vkPipeline = Cast<VulkanPipeline>(pipeline);
		vkPipeline->BindImmediate(cmd);

		auto bindings = CreateRef<FVulkanComputeBindings>(vkPipeline);

		builder(*bindings);

		auto &images = bindings->GetBoundImages();
		for (auto& [img, isStorage] : images)
		{
			auto vkImg = img.Texture->GetNativeHandle().As<VulkanImage>();
			ImageSubresource sub{};
			sub.MipLevel = img.BaseMip;
			sub.LevelCount = img.LevelCount;
			sub.BaseArrayLayer = img.BaseLayer;
			sub.LayerCount = img.LayerCount;
			

			if (isStorage && (img.Access == EImageAccess::WRITE || img.Access == EImageAccess::READ_WRITE))
				vkImg->Transition(cmd, ImageState::ComputeWrite(), sub);
			else
				vkImg->Transition(cmd, ImageState::ShaderRead(), sub);
		}

		bindings->BindImmediate(cmd);

		cmd.dispatch(size.x, size.y, size.z);

		for (auto & [img, isStorage] : images)
		{
			if (!isStorage)
				continue;

			auto vkImg = img.Texture->GetNativeHandle().As<VulkanImage>();
			ImageSubresource sub{};
			sub.MipLevel = img.BaseMip;
			sub.LevelCount = img.LevelCount;
			sub.BaseArrayLayer = img.BaseLayer;
			sub.LayerCount = img.LayerCount;
			
			vkImg->Transition(cmd, ImageState::ShaderRead(), sub);
		}

		cmd.endDebugUtilsLabelEXT();
		cmd.end();

		vk::FenceCreateInfo fenceInfo{};
		vk::Fence fence = (*mDevice).createFence(fenceInfo);

		vk::SubmitInfo submitInfo({}, {}, cmd);
		auto &queue = VulkanBackend::GetQueueFamilies().GraphicsQueue;
		queue.submit(submitInfo);
		
		AsyncComputeHandle handle{};
		handle.Fence = new vk::Fence(fence);
		handle.CommandBuffer = new vk::CommandBuffer(cmd);
		handle.Bindings = bindings;

		QueueDeletion(
			[&, handle](uint32_t)
			{
				auto device = (*mDevice);
				auto &commandPool = VulkanBackend::GetImmediateCommandPool();
				auto fencePtr = static_cast<vk::Fence *>(handle.Fence);
				auto cmdPtr = static_cast<vk::CommandBuffer *>(handle.CommandBuffer);
				device.destroyFence(*fencePtr);
				device.freeCommandBuffers(commandPool, *cmdPtr);
			});

		return handle;
	}

} // namespace BHive
