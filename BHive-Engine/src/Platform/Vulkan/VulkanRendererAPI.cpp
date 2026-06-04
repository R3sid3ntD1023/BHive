#include "VulkanPipeline.h"
#include "VulkanShader.h"
#include "VulkanVertexArray.h"
#include "VulkanRendererAPI.h"
#include "VulkanSwapChain.h"
#include "VulkanConverters.h"
#include "gfx/BufferBase.h"
#include "VulkanFramebuffer.h"
#include "VulkanUtils.h"
#include "VulkanSetManager.h"
#include "systems/GlobalSetRegistry.h"
#include "systems/MaterialSetRegistry.h"
#include "textures/VulkanImage.h"
#include "pass/ComputeBindings.h"

namespace BHive
{
	
	struct FVulkanAsycComputePass : public FAsyncPass
	{
		vk::Device Device;
		vk::CommandBuffer Cmd;
		vk::Fence Fence;
		vk::Queue Queue;
		Ref<FComputeBindings> Bindings;

		bool IsDone() override { return Device.getFenceStatus(Fence) == vk::Result::eSuccess;}

		void Wait() override { auto result =  Device.waitForFences(Fence, VK_TRUE, UINT64_MAX);}

		void Destroy() override
		{
			Wait();
			Device.destroyFence(Fence);
			Device.freeCommandBuffers(VulkanBackend::GetImmediateCommandPool(), Cmd);
			delete this;
		}
	};

	VulkanRendererAPI::VulkanRendererAPI()
		: mDevice(VulkanBackend::GetLogicalDevice())
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

	vk::Result VulkanRendererAPI::RenderFrame(VulkanSwapChain *swapChain)
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

		return ExecuteFinalGraph(swapChain, mergedUpdates, finalGraph);
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


	void VulkanRendererAPI::QueueDeletion(FQeueuDeflectionFunc&& fn)
	{
		mDeletionQueue.emplace(mCompletedFrame, std::move(fn));
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

	void VulkanRendererAPI::ResetFrameIndex()
	{
		mCurrentFrame = 0;
		mCompletedFrame = 0;

		mSubmittedGraphs.clear();
		mSubmittedUpdates.clear();

		while (!mDeletionQueue.empty())
			mDeletionQueue.pop();
	}

	void VulkanRendererAPI::ProcessDeletionQueue(uint32_t frame)
	{
		while (!mDeletionQueue.empty())
		{
			auto & del = mDeletionQueue.front();

			if (frame > del.Frame)
				del.Fn(frame);
			mDeletionQueue.pop();
		}
	}

	vk::Result VulkanRendererAPI::ExecuteFinalGraph(VulkanSwapChain *swapChain, FResourceUpdateList &updates, const RenderGraph &graph)
	{
		auto current_frame = mCurrentFrame;
		auto& cmd = VulkanBackend::GetCommandBuffer(current_frame);

		swapChain->WaitForFence(current_frame);

		ProcessDeletionQueue(current_frame);

		cmd.reset();

		auto [result, imageIndex] = swapChain->AquireNextImage(current_frame);

		if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
			return result;

		vk::CommandBufferBeginInfo beginInfo{};
		cmd.begin(beginInfo);

		vk::DebugUtilsLabelEXT label_info("Main Pass", std::array<float, 4>{0.0f, 1.0f, 0.0f, 1.0f});
		cmd.beginDebugUtilsLabelEXT(label_info);

		FVulkanRendererContext vk_ctx{cmd, current_frame, imageIndex};

		GetSubSystem<GlobalSetRegistry>().UpdatePerFrame(current_frame);
		GetSubSystem<MaterialSetRegistry>().UpdatePerFrame(current_frame);

		updates.Execute(vk_ctx);

		for (auto &pass : graph.GetPasses())
		{
			vk::DebugUtilsLabelEXT debugInfo(pass.Name.c_str(), {1, 0, 0, 1});

			cmd.beginDebugUtilsLabelEXT(debugInfo);

			switch (pass.Type)
			{
			case EPassType::SwapChain:
				ExecuteSwapChainPass(pass, vk_ctx, swapChain);
				break;
			case EPassType::OffScreen:
				ExecuteOffScreenPass(pass, vk_ctx);
				break;
			default:
				break;
			}
			cmd.endDebugUtilsLabelEXT();
		}

		cmd.endDebugUtilsLabelEXT();
		cmd.end();

		result = swapChain->Present(cmd, imageIndex, current_frame);

		mCompletedFrame = current_frame;

		if (result == vk::Result::eSuccess)
		{
			mCurrentFrame = (mCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
		}

		return result;
	}

	void VulkanRendererAPI::ExecuteSwapChainPass(const FRenderGraphPass &pass, FVulkanRendererContext &ctx, VulkanSwapChain* swapChain)
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

	void VulkanRendererAPI::SetCurrentContext(WindowContext *ctx)
	{
		mCurrentContext = ctx;
	}

	void VulkanRendererAPI::ClearColor(FRenderGraphPass *pass, float r, float g, float b, float a)
	{
		pass->CommandList.Push("SetClearColor", [=](IRendererContext &) { mClearColor = {r, g, b, a}; });
	}

	void VulkanRendererAPI::Clear(FRenderGraphPass *pass, ClearMask mask)
	{
		
	}

	void VulkanRendererAPI::SetLineWidth(FRenderGraphPass *pass, float width)
	{
		pass->CommandList.Push(
			"Set Line Width",
			[=](IRendererContext &ctx)
			{
				auto &vk_ctx = CastRef<FVulkanRendererContext>(ctx);
				vk_ctx.CommandBuffer.setLineWidth(width);
			});
	}

	void VulkanRendererAPI::SetViewport(FRenderGraphPass *pass, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
	{
		pass->CommandList.Push(
			"Set Viewport",
			[=](IRendererContext &ctx)
			{
				auto &vk_ctx = CastRef<FVulkanRendererContext>(ctx);
				vk_ctx.CommandBuffer.setViewport(0, vk::Viewport((float)x, (float)(y + h), (float)w, -(float)h, 0.0f, 1.0f));
				vk_ctx.CommandBuffer.setScissor(0, vk::Rect2D({(int32_t)x, (int32_t)y}, vk::Extent2D(w, h)));
			});
	}

	void VulkanRendererAPI::DrawArrays(FRenderGraphPass *pass, ETopologyMode mode, VertexArray* vao, uint32_t count)
	{
		vao->Bind();

		auto topology = ToVkTopology(mode);

		pass->CommandList.Push(
			"Draw Arrays",
			[=](IRendererContext &ctx)
			{
				auto &vk_ctx = CastRef<FVulkanRendererContext>(ctx);
				vk_ctx.CommandBuffer.setPrimitiveTopology(topology);
				vk_ctx.CommandBuffer.draw(count, 1, 0, 0);
			});
	}

	void VulkanRendererAPI::DrawElements(FRenderGraphPass *pass, ETopologyMode mode, VertexArray* vao, uint32_t count)
	{
		vao->Bind();
		auto index_buffer =vao->GetIndexBuffer();
		auto index_count = count ? count : index_buffer->GetCount();
		auto topology = ToVkTopology(mode);

		pass->CommandList.Push(
			"Draw Elements",
			[=](IRendererContext &ctx)
			{
				auto &vk_ctx = static_cast<const FVulkanRendererContext &>(ctx);
				vk_ctx.CommandBuffer.setPrimitiveTopology(topology);
				vk_ctx.CommandBuffer.drawIndexed(index_count, 1, 0, 0, 0);
			});
	}

	void
	VulkanRendererAPI::DrawElementsBaseVertex(FRenderGraphPass *pass, ETopologyMode mode, VertexArray* vao, uint32_t start, uint32_t start_index, uint32_t count, uint32_t instance_count)
	{
		vao->Bind();
		auto index_buffer = vao->GetIndexBuffer();
		auto index_count = count ? count : index_buffer->GetCount();
		auto topology = ToVkTopology(mode);

		pass->CommandList.Push(
			"Draw Elements",
			[=](IRendererContext &ctx)
			{
				auto &vk_ctx = static_cast<const FVulkanRendererContext &>(ctx);
				vk_ctx.CommandBuffer.setPrimitiveTopology(topology);
				vk_ctx.CommandBuffer.drawIndexed(index_count, instance_count, start_index, start, 0);
			});
	}

	void VulkanRendererAPI::DrawElementsRanged(FRenderGraphPass *pass, ETopologyMode mode, VertexArray* vao, uint32_t start, uint32_t end, uint32_t count)
	{
		
	}

	void VulkanRendererAPI::DrawElementsInstanced(FRenderGraphPass *pass, ETopologyMode mode, VertexArray* vao, uint32_t instances, uint32_t count)
	{
	
	}

	void VulkanRendererAPI::MultiDrawElementsIndirect(FRenderGraphPass *pass, ETopologyMode mode, BufferBase* indirect, VertexArray* vao, size_t drawCount, size_t stride)
	{
		vao->Bind();

		auto buffer = indirect->GetNativeHandle().As<AllocatedBuffer>()->GetBuffer();
		auto topology = ToVkTopology(mode);

		pass->CommandList.Push("Multi Draw Elements Indirect", [buffer, topology, drawCount, stride](IRendererContext &ctx)
		{		
			auto &vk_ctx = static_cast<const FVulkanRendererContext &>(ctx);
			vk_ctx.CommandBuffer.setPrimitiveTopology(topology);
			vk_ctx.CommandBuffer.drawIndexedIndirect(buffer, 0, drawCount, stride);
		});

		vao->UnBind();
	}

	void VulkanRendererAPI::ColorMask(FRenderGraphPass *pass, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
	{	
	}


	Ref<FComputeBindings> VulkanRendererAPI::CreateComputeBindings(Pipeline* pipeline)
	{
		auto vkPipeline = Cast<VulkanPipeline>(pipeline);
		return CreateRef<FVulkanComputeBindings>(vkPipeline);
	}

	FAsyncPass* VulkanRendererAPI::ExecuteComputePass(Pipeline* pipeline, const glm::uvec3 & size, const FComputeFunc &builder)
	{
		SingleTimeCommand single_cmd{};
		auto& cmd = single_cmd.Get();

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
			ImageSubresource sub{img.BaseMip, img.LevelCount, img.BaseLayer, img.LayerCount};
			
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
			ImageSubresource sub{img.BaseMip, img.LevelCount, img.BaseLayer, img.LayerCount};
			vkImg->Transition(cmd, ImageState::ShaderRead(), sub);
		}

		cmd.endDebugUtilsLabelEXT();
		
		/*Scope<FVulkanAsycComputePass> pass = CreateScope<FVulkanAsycComputePass>();
		
		pass->Fence = fence;
		pass->Cmd = cmd;
		pass->Device = mDevice;
		pass->Queue = queue;
		pass->Bindings = bindings;

		auto raw = pass.get();
		mComputePasses.emplace_back(std::move(pass));

		QueueDeletion([raw](uint32_t) { raw->Destroy(); });

		return raw;*/
		return nullptr;
	}

	void VulkanRendererAPI::ExecuteTransferPass(FTransferFunc &&builder)
	{
		SingleTimeCommand single_cmd{};
		auto& cmd = single_cmd.Get();

		vk::DebugUtilsLabelEXT labelInfo("Transfer Pass", {1, 0, .5, 1});
		cmd.beginDebugUtilsLabelEXT(labelInfo);

		FVulkanTransferContext ctx{cmd};

		builder(ctx);

		cmd.endDebugUtilsLabelEXT();
	}

} // namespace BHive
