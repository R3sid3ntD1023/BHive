#include "VulkanRendererAPI.h"
#include "VulkanSwapChain.h"
#include "VulkanImage.h"
#include "VulkanCommandTranslater.h"
#include "VulkanFramebuffer.h"
#include "VulkanBackend.h"
#include "gfx/renderers/Renderer.h"
#include "VulkanImage.h"
#include "VulkanQuery.h"

namespace BHive
{
	namespace utils
	{
		vk::AttachmentLoadOp ToLoad(EAttachmentLoadState state)
		{
			switch (state)
			{
			case BHive::EAttachmentLoadState::DontCare:
				return vk::AttachmentLoadOp::eDontCare;
			case BHive::EAttachmentLoadState::Clear:
				return vk::AttachmentLoadOp::eClear;
			case BHive::EAttachmentLoadState::Load:
				return vk::AttachmentLoadOp::eLoad;
			default:
				return vk::AttachmentLoadOp::eNone;
			}
		}

		vk::AttachmentStoreOp ToStore(EAttachmentStoreState state)
		{
			switch (state)
			{
			case BHive::EAttachmentStoreState::DontCare:
				return vk::AttachmentStoreOp::eDontCare;
			case BHive::EAttachmentStoreState::Store:
				return vk::AttachmentStoreOp::eStore;
			default:
				return vk::AttachmentStoreOp::eNone;
			}
		}
	} // namespace utils

	void VulkanRendererAPI::Init()
	{
		mBackend = CreateScope<VulkanBackend>();
		mBackend->Init();
	}

	void VulkanRendererAPI::Shutdown()
	{
		LOG_TRACE("RendererAPI Shutdown Called")

		FlushDeletionQueue();

		mBackend->Shutdown();
	}

	vk::Result VulkanRendererAPI::RenderFrame(VulkanSwapChain *swapChain)
	{

		RenderGraph finalGraph;
		for (auto &g : mSubmittedGraphs)
			finalGraph.Append(g);

		mSubmittedGraphs.clear();

		if (finalGraph.Empty())
		{
			return vk::Result::eSuccess;
		}

		return ExecuteFinalGraph(swapChain, finalGraph);
	}

	void VulkanRendererAPI::SubmitGraph(const RenderGraph &graph)
	{
		if (!graph.Empty())
			mSubmittedGraphs.emplace_back(graph);
	}

	void VulkanRendererAPI::QueueDeletion(FQeueuDeletionFunc &&fn)
	{
		mDeletionQueue.emplace_back(mCompletedFrame, std::move(fn));
	}

	void VulkanRendererAPI::ResetFrameIndex()
	{
		mCurrentFrame = 0;
		mCompletedFrame = 0;

		mSubmittedGraphs.clear();
		mDeletionQueue.clear();
	}

	void VulkanRendererAPI::ProcessDeletionQueue(uint32_t frame)
	{
		while (!mDeletionQueue.empty())
		{
			auto &del = mDeletionQueue.front();

			if (frame >= del.Frame)
				del.Fn(frame);
			else
				break;

			mDeletionQueue.erase(mDeletionQueue.begin());
		}
	}

	vk::Result VulkanRendererAPI::ExecuteFinalGraph(VulkanSwapChain *swapChain, RenderGraph &graph)
	{
		auto current_frame = mCurrentFrame;
		auto &cmd = VulkanBackend::GetCommandBuffer(current_frame);

		swapChain->WaitForFence(current_frame);

		ProcessDeletionQueue(mCompletedFrame);

		cmd.reset();

		auto [result, imageIndex] = swapChain->AquireNextImage(current_frame);

		if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
			return result;

		auto vk_ctx = BuildContext(cmd, current_frame, imageIndex, 0);

		vk::CommandBufferBeginInfo beginInfo{};
		cmd.begin(beginInfo);

		for (auto &pass : graph.GetPasses())
		{
			ExecutePass(pass, vk_ctx, swapChain);
		}

		cmd.end();

		result = swapChain->Present(cmd, imageIndex, current_frame);

		mCompletedFrame = current_frame;

		if (result == vk::Result::eSuccess)
		{
			mCurrentFrame = (mCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
		}

		return result;
	}

	void VulkanRendererAPI::ExecutePass(const FPass &pass, FVulkanRendererContext &ctx, VulkanSwapChain *swapChain)
	{
		auto &cmd = ctx.CommandBuffer;
		auto state = pass.State;

		vk::DebugUtilsLabelEXT label(pass.Name.c_str(), {1.0f, .5f, 0.0f, 1.0f});
		cmd.beginDebugUtilsLabelEXT(label);

		for (auto &phase : pass.Phases)
		{
			CreateBarriers(phase.CommandList, ctx);

			TransitionImages(phase, cmd);

			if (phase.Type == EPhaseType::Graphics)
			{
				if (pass.Type == EPassType::Present)
					BeginSwapChainRendering(state, phase, ctx, swapChain);
				else
					BeginOffScreenRendering(state, phase, ctx);
			}

			VulkanCommandTranslator::ExecuteCommandList(pass, phase, ctx);

			if (phase.Type == EPhaseType::Graphics)
			{
				EndRendering(ctx);

				if (pass.Type == EPassType::Present)
					TransitionSwapChainToPresent(ctx, swapChain);
			}
		}

		cmd.endDebugUtilsLabelEXT();
	}

	void VulkanRendererAPI::TransitionImages(const FPhase &phase, vk::raii::CommandBuffer &cmd)
	{
		for (auto &imgInfo : phase.Images)
		{
			auto tex = imgInfo.Texture;
			auto name = tex->GetInfo().DebugName;
			auto vkImg = tex->GetNativeHandle().As<VulkanImage>();

			ImageState oldState = vkImg->GetState(imgInfo.Range.BaseMipLevel, imgInfo.Range.BaseArrayLayer);
			ImageState newState = ImageState::ToImageState(imgInfo.Access);

			if (oldState.IsUndefined || oldState != newState)
			{
				vkImg->Transition(cmd, newState, imgInfo.Range);
			}
		}
	}

	void VulkanRendererAPI::BeginSwapChainRendering(const FPassState &state, const FPhase &phase, FVulkanRendererContext &ctx, VulkanSwapChain *swapChain)
	{
		vk::ClearColorValue clearColor(state.Color.ClearColor.r, state.Color.ClearColor.g, state.Color.ClearColor.b, state.Color.ClearColor.a);
		vk::ClearDepthStencilValue depthValue = {1.0f, 0};

		swapChain->BeginRendering(ctx.CommandBuffer, ctx.ImageIndex, clearColor, depthValue);
	}

	void VulkanRendererAPI::BeginOffScreenRendering(const FPassState &state, const FPhase &phase, FVulkanRendererContext &ctx)
	{
		auto fbo = Cast<VulkanFramebuffer>(phase.FBO);
		if (fbo)
		{
			const auto range = phase.ColorRange;
			auto &cmd = ctx.CommandBuffer;

			VulkanFramebuffer::RenderInfo renderInfo;
			renderInfo.ClearColor = vk::ClearColorValue(state.Color.ClearColor.r, state.Color.ClearColor.g, state.Color.ClearColor.b, state.Color.ClearColor.a);
			renderInfo.ClearDepthValue = vk::ClearDepthStencilValue(1.0f, 0);
			renderInfo.ColorLoadOp = utils::ToLoad(state.Color.LoadOP);
			renderInfo.ColorStoreOp = utils::ToStore(state.Color.StoreOP);
			renderInfo.DepthLoadOp = utils::ToLoad(state.Depth.LoadOP);
			renderInfo.DepthStoreOp = utils::ToStore(state.Depth.StoreOP);
			renderInfo.ColorRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, range.BaseMipLevel, range.LevelCount, range.BaseArrayLayer, range.LayerCount);

			fbo->BeginRendering(ctx.CommandBuffer, renderInfo);
		}
	}

	void VulkanRendererAPI::TransitionSwapChainToPresent(FVulkanRendererContext &ctx, VulkanSwapChain *swapChain)
	{
		swapChain->EndRendering(ctx.CommandBuffer, ctx.ImageIndex);
	}

	void VulkanRendererAPI::EndRendering(FVulkanRendererContext &ctx)
	{
		ctx.CommandBuffer.endRendering();
	}

	void VulkanRendererAPI::CreateBarriers(const FRenderCommandList &list, FVulkanRendererContext &ctx)
	{
		VulkanCommandTranslator::CreateBarriers(list, ctx);
	}

	void VulkanRendererAPI::FlushDeletionQueue()
	{
		while (!mDeletionQueue.empty())
		{
			auto &del = mDeletionQueue.front();
			del.Fn(0);
			mDeletionQueue.erase(mDeletionQueue.begin());
		}
	}

	FVulkanRendererContext VulkanRendererAPI::BuildContext(vk::raii::CommandBuffer &cmd, uint32_t frame, uint32_t imageIndex, uint32_t viewIndex)
	{
		FVulkanRendererContext ctx(cmd, frame, imageIndex, viewIndex);
		return ctx;
	}

} // namespace BHive
