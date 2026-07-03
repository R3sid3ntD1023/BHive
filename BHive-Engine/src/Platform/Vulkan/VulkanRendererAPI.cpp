#include "VulkanRendererAPI.h"
#include "VulkanSwapChain.h"
#include "VulkanImage.h"
#include "VKCommandTranslater.h"
#include "VulkanFramebuffer.h"
#include "VulkanBackend.h"
#include "gfx/renderers/Renderer.h"

namespace BHive
{
	
	VulkanRendererAPI::VulkanRendererAPI()
		: mDevice(VulkanBackend::GetLogicalDevice())
	{
	}

	void VulkanRendererAPI::Init()
	{
		mDescriptorPoolManager.Init(mDevice);
	}

	void VulkanRendererAPI::Shutdown()
	{
		LOG_TRACE("RendererAPI Shutdown Called")

		mDeletionQueue.clear();

		mDescriptorPoolManager.Shutdown();

		VulkanBackend::Get().Shutdown();
	}

	void VulkanRendererAPI::WaitIdle()
	{
		LOG_TRACE("RendererAPI Wait Idle")
		VulkanBackend::GetLogicalDevice().waitIdle();
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

		

		//Print all passes -> phases -> cmds to console
		//finalGraph.DebugPrint();

		return ExecuteFinalGraph(swapChain, finalGraph);
	}

	void VulkanRendererAPI::SubmitGraph(const RenderGraph &graph)
	{
		if (!graph.Empty())
			mSubmittedGraphs.emplace_back(graph);
	}


	void VulkanRendererAPI::QueueDeletion(FQeueuDeflectionFunc&& fn)
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
			auto & del = mDeletionQueue.front();

			if (frame > del.Frame)
				del.Fn(frame);

			mDeletionQueue.erase(mDeletionQueue.begin());
		}
	}

	vk::Result VulkanRendererAPI::ExecuteFinalGraph(VulkanSwapChain *swapChain, RenderGraph &graph)
	{
		auto current_frame = mCurrentFrame;
		auto& cmd = VulkanBackend::GetCommandBuffer(current_frame);

		swapChain->WaitForFence(current_frame);

		ProcessDeletionQueue(mCompletedFrame);

		cmd.reset();

		auto [result, imageIndex] = swapChain->AquireNextImage(current_frame);

		if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
			return result;

		mDescriptorPoolManager.ResetFrame(current_frame);

		FVulkanRendererContext vk_ctx{cmd, current_frame, imageIndex, 0};

		

		vk::CommandBufferBeginInfo beginInfo{};
		cmd.begin(beginInfo);

		for (auto &pass : graph.GetPasses())
		{
			if (pass.HasView())
			{
				auto camera = Renderer::Get().GetGlobalResources().Find("Camera");
				if (camera)
					camera->BufferRef->SetData(&pass.GetView(), sizeof(FView));
			}

			for (auto phase : pass.Phases)
			{
				CreateBarriers(phase.CommandList, vk_ctx);
			}

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
		for (auto& phase : pass.Phases)
		{
			TransitionImages(phase, ctx.CommandBuffer);

			if (phase.Type == EPhaseType::Graphics)
			{
				if (pass.Type == EPassType::Present)
					BeginSwapChainRendering(phase, ctx, swapChain);
				else
					BeginOffScreenRendering(pass, phase, ctx);
			}

			ExecuteCommandList(phase.CommandList, ctx);
			
			if (phase.Type == EPhaseType::Graphics)
			{
				EndRendering(phase, ctx);

				if (pass.Type == EPassType::Present)
					TransitionSwapChainToPresent(ctx, swapChain);
			}
			
		}
		
	}

	void VulkanRendererAPI::TransitionImages(const FPhase &phase, vk::raii::CommandBuffer &cmd)
	{
		for (auto &imgInfo : phase.Images)
		{
			auto tex = imgInfo.Texture;
			auto vkImg = tex->GetNativeHandle().As<VulkanImage>();

			ImageState oldstate = vkImg->GetState(imgInfo.Range.BaseMipLevel, imgInfo.Range.BaseArrayLayer);
			ImageState newState = ImageState::ToImageState(imgInfo.Access);

			if (oldstate != newState)
			{
				vkImg->Transition(cmd, newState, imgInfo.Range);
			}
		}
	}

	void VulkanRendererAPI::ExecuteCommandList(const FRenderCommandList &list, FVulkanRendererContext &ctx)
	{
		VKCommandTranslator::ExecuteCommandList(list, ctx);
	}

	void VulkanRendererAPI::BeginSwapChainRendering(const FPhase &phase, FVulkanRendererContext &ctx, VulkanSwapChain *swapChain)
	{
		auto &image = swapChain->GetImage(ctx.ImageIndex);
		auto &depth = swapChain->GetDepthImage();
		auto extent = swapChain->GetExtent();
		auto &cmd = ctx.CommandBuffer;

		// Color: Undefined/ShaderRead/etc → ColorAttachment
		image.Transition(cmd, ImageState::ColorAttachment());

		// Depth: Undefined/ShaderRead/etc → DepthStencilAttachment
		depth.Transition(cmd, ImageState::DepthStencilAttachment());

		vk::ClearValue clearColor(VKCommandTranslator::GetClearColor());
		vk::ClearValue clearDepth(VKCommandTranslator::GetDepthStencilValue());

		vk::RenderingAttachmentInfo attachmentInfo(
			image.Native().GetView(0, 0, 0), vk::ImageLayout::eColorAttachmentOptimal, {}, {}, vk::ImageLayout::eUndefined, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, clearColor);

		vk::RenderingAttachmentInfo depth_attachment_info(
			depth.Native().GetView(0, 0, 0), vk::ImageLayout::eDepthStencilAttachmentOptimal, {}, {}, vk::ImageLayout::eUndefined, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eDontCare,
			clearDepth);

		vk::RenderingInfo renderingInfo({}, vk::Rect2D({0, 0}, extent), 1, 0, attachmentInfo, &depth_attachment_info);
		cmd.beginRendering(renderingInfo);

	}

	void VulkanRendererAPI::BeginOffScreenRendering(const FPass& pass, const FPhase &phase, FVulkanRendererContext &ctx)
	{
		auto fbo = Cast<VulkanFramebuffer>(phase.FBO);
		if(phase.Type == EPhaseType::Graphics && fbo)
		{
			auto depth = fbo->GetDepthAttachment();
			auto numColorAttachments = fbo->GetNumColorAttachments();
			auto clearColor = VKCommandTranslator::GetClearColor();
			auto depthStencilValue = VKCommandTranslator::GetDepthStencilValue();
			auto face = fbo->GetCurrentFace();
			auto &cmd = ctx.CommandBuffer;
			vk::AttachmentLoadOp loadOp = pass.Type == EPassType::Overlay ? vk::AttachmentLoadOp::eLoad : vk::AttachmentLoadOp::eClear;

			std::vector<vk::RenderingAttachmentInfo> color_infos;
			color_infos.reserve(numColorAttachments);

			for (size_t i = 0; i < numColorAttachments; i++)
			{
				auto attachment = fbo->GetColorAttachment(i);
				auto &spec = fbo->GetColorAttachmentSpecs(i);
				auto vkTex = reinterpret_cast<VkImageView>(attachment->GetRenderView(face, spec.MipLevel).AsRaw());

				auto info = vk::RenderingAttachmentInfo(vkTex, vk::ImageLayout::eColorAttachmentOptimal, {}, {}, vk::ImageLayout::eUndefined, loadOp, vk::AttachmentStoreOp::eStore, clearColor);

				color_infos.emplace_back(info);
			}

			vk::RenderingAttachmentInfo *depthPtr = nullptr;

			if (depth)
			{
				auto &spec = fbo->GetDepthAttachmentSpecs();
				auto vkTex = reinterpret_cast<VkImageView>(depth->GetRenderView(face, spec.MipLevel).AsRaw());

				auto depthInfo = vk::RenderingAttachmentInfo(
					vkTex, vk::ImageLayout::eDepthStencilAttachmentOptimal, {}, {}, vk::ImageLayout::eUndefined, loadOp, vk::AttachmentStoreOp::eStore, depthStencilValue);

				depthPtr = &depthInfo;
			}

			auto &spec = fbo->GetSpecification();
			auto rect = vk::Rect2D({0, 0}, {spec.Size.x, spec.Size.y});
			auto info = vk::RenderingInfo({}, rect, 1, 0, color_infos, depthPtr);

			cmd.beginRendering(info);

			vk::Viewport viewport(0.f, (float)spec.Size.y, (float)spec.Size.x, -(float)spec.Size.y, 0.0f, 1.0f);
			vk::Rect2D scissor({0, 0}, {spec.Size.x, spec.Size.y});

			cmd.setViewport(0, viewport);
			cmd.setScissor(0, scissor);
		}
	}

	void VulkanRendererAPI::TransitionSwapChainToPresent(FVulkanRendererContext &ctx, VulkanSwapChain *swapChain)
	{
		auto &image = swapChain->GetImage(ctx.ImageIndex);
		image.Transition(ctx.CommandBuffer, ImageState::Present());
	}

	void VulkanRendererAPI::EndRendering(const FPhase &phase, FVulkanRendererContext &ctx)
	{
		if(phase.Type == EPhaseType::Graphics)
		{
			ctx.CommandBuffer.endRendering();
		}
	}

	void VulkanRendererAPI::CreateBarriers(const FRenderCommandList &list, FVulkanRendererContext &ctx)
	{
		VKCommandTranslator::CreateBarriers(list, ctx);
	}

	void VulkanRendererAPI::SetCurrentContext(WindowContext *ctx)
	{
		mCurrentContext = ctx;
	}

} // namespace BHive
