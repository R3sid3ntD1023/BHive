#include "VulkanRendererAPI.h"
#include "VulkanSwapChain.h"
#include "VulkanImage.h"
#include "VulkanCommandTranslater.h"
#include "VulkanFramebuffer.h"
#include "VulkanBackend.h"
#include "gfx/renderers/Renderer.h"
#include "VulkanImage.h"

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

		FlushDeletionQueue();

		VulkanBackend::Get().Shutdown();

		mDescriptorPoolManager.Shutdown();
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

		// Print all passes -> phases -> cmds to console
		// finalGraph.DebugPrint();

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

			if (frame > del.Frame)
				del.Fn(frame);

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
			/*if (pass.HasView())
			{
				auto camera = Renderer::Get().GetGlobalResources().Find("Camera");
				if (camera)
				{
					camera->BufferRef->SetData(&pass.GetView(), sizeof(FView));
				}
			}*/

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
		// LOG_TRACE("Pass: {}", pass.Name);

		auto &cmd = ctx.CommandBuffer;
		auto state = pass.State;

		vk::DebugUtilsLabelEXT label(pass.Name.c_str(), {1.0f, .5f, 0.0f, 1.0f});
		cmd.beginDebugUtilsLabelEXT(label);

		for (auto &phase : pass.Phases)
		{
			// LOG_TRACE("\tPhase {}", phase.Name);

			TransitionImages(phase, cmd);

			if (phase.Type == EPhaseType::Graphics)
			{
				if (pass.Type == EPassType::Present)
					BeginSwapChainRendering(state, phase, ctx, swapChain);
				else
					BeginOffScreenRendering(state, phase, ctx);
			}

			auto numAtatchments = phase.FBO ? phase.FBO->GetNumColorAttachments() : 0;
			ExecuteCommandList(phase.CommandList, ctx, numAtatchments);

			if (phase.Type == EPhaseType::Graphics)
			{
				EndRendering(phase, ctx);

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

	void VulkanRendererAPI::ExecuteCommandList(const FRenderCommandList &list, FVulkanRendererContext &ctx, uint32_t numAttachments)
	{
		VulkanCommandTranslator::ExecuteCommandList(list, ctx, numAttachments);
	}

	void VulkanRendererAPI::BeginSwapChainRendering(const FPassState &state, const FPhase &phase, FVulkanRendererContext &ctx, VulkanSwapChain *swapChain)
	{
		auto &image = swapChain->GetImage(ctx.ImageIndex);
		auto &depth = swapChain->GetDepthImage();
		auto extent = swapChain->GetExtent();
		auto &cmd = ctx.CommandBuffer;

		// Color: Undefined/ShaderRead/etc → ColorAttachment
		image.Transition(cmd, ImageState::ColorAttachment());

		// Depth: Undefined/ShaderRead/etc → DepthStencilAttachment
		depth.Transition(cmd, ImageState::DepthStencilAttachment());

		auto clearColor = vk::ClearColorValue(state.Color.ClearColor.r, state.Color.ClearColor.g, state.Color.ClearColor.b, state.Color.ClearColor.a);

		vk::RenderingAttachmentInfo attachmentInfo(
			image.Native().GetView(0, 0, 0), vk::ImageLayout::eColorAttachmentOptimal, {}, {}, vk::ImageLayout::eUndefined, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, clearColor);

		vk::RenderingAttachmentInfo depth_attachment_info(
			depth.Native().GetView(0, 0, 0), vk::ImageLayout::eDepthStencilAttachmentOptimal, {}, {}, vk::ImageLayout::eUndefined, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eDontCare,
			vk::ClearDepthStencilValue(1.0f, 0));

		vk::RenderingInfo renderingInfo({}, vk::Rect2D({0, 0}, extent), 1, 0, attachmentInfo, &depth_attachment_info);
		cmd.beginRendering(renderingInfo);

		vk::Viewport viewport(0.f, (float)extent.height, (float)extent.width, -(float)extent.height, 0.0f, 1.0f);
		vk::Rect2D scissor({0, 0}, extent);

		cmd.setViewportWithCount(viewport);
		cmd.setScissorWithCount(scissor);
	}

	void VulkanRendererAPI::BeginOffScreenRendering(const FPassState &state, const FPhase &phase, FVulkanRendererContext &ctx)
	{
		auto fbo = Cast<VulkanFramebuffer>(phase.FBO);
		if (fbo)
		{
			const auto range = phase.ColorRange;
			const auto numColorAttachments = fbo->GetNumColorAttachments();

			auto depth = fbo->GetDepthAttachment();

			auto &cmd = ctx.CommandBuffer;

			std::vector<vk::RenderingAttachmentInfo> color_infos;
			color_infos.reserve(numColorAttachments);

			{
				vk::AttachmentLoadOp loadOp = utils::ToLoad(state.Color.LoadOP);
				vk::AttachmentStoreOp storeOP = utils::ToStore(state.Color.StoreOP);
				auto clearColor = vk::ClearColorValue(state.Color.ClearColor.r, state.Color.ClearColor.g, state.Color.ClearColor.b, state.Color.ClearColor.a);

				for (size_t i = 0; i < numColorAttachments; i++)
				{
					auto attachment = fbo->GetColorAttachment(i);
					auto &spec = fbo->GetColorAttachmentSpecs(i);
					auto view = Cast<IVulkanTextureInterface>(attachment)->ResolveRenderView(range.BaseArrayLayer, range.BaseMipLevel);

					auto info = vk::RenderingAttachmentInfo(view, vk::ImageLayout::eColorAttachmentOptimal, {}, {}, vk::ImageLayout::eColorAttachmentOptimal, loadOp, storeOP, clearColor);

					color_infos.emplace_back(info);
				}
			}

			vk::RenderingAttachmentInfo depthInfo{};
			vk::RenderingAttachmentInfo *depthPtr = nullptr;

			if (depth)
			{
				vk::AttachmentLoadOp loadOp = utils::ToLoad(state.Depth.LoadOP);
				vk::AttachmentStoreOp storeOP = utils::ToStore(state.Depth.StoreOP);

				auto &spec = fbo->GetDepthAttachmentSpecs();
				auto view = Cast<IVulkanTextureInterface>(depth)->ResolveRenderView(0, 0);

				depthInfo = vk::RenderingAttachmentInfo(
					view, vk::ImageLayout::eDepthStencilAttachmentOptimal, {}, {}, vk::ImageLayout::eDepthStencilAttachmentOptimal, loadOp, storeOP, vk::ClearDepthStencilValue(1.0f, 0));

				depthPtr = &depthInfo;
			}

			auto &spec = fbo->GetSpecification();
			glm::uvec2 baseSize = spec.Size;
			glm::uvec2 mipSize = {std::max(baseSize.x >> range.BaseMipLevel, 1u), std::max(baseSize.y >> range.BaseMipLevel, 1u)};

			auto rect = vk::Rect2D({0, 0}, {mipSize.x, mipSize.y});
			auto info = vk::RenderingInfo({}, rect, 1, 0, color_infos, depthPtr);

			cmd.beginRendering(info);

			vk::Viewport viewport(0.f, (float)mipSize.y, (float)mipSize.x, -(float)mipSize.y, 0.0f, 1.0f);
			vk::Rect2D scissor({0, 0}, {mipSize.x, mipSize.y});

			cmd.setViewportWithCount(viewport);
			cmd.setScissorWithCount(scissor);
		}
	}

	void VulkanRendererAPI::TransitionSwapChainToPresent(FVulkanRendererContext &ctx, VulkanSwapChain *swapChain)
	{
		auto &image = swapChain->GetImage(ctx.ImageIndex);
		image.Transition(ctx.CommandBuffer, ImageState::Present());
	}

	void VulkanRendererAPI::EndRendering(const FPhase &phase, FVulkanRendererContext &ctx)
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
		ctx.ModelBuffer = Renderer::Get().GetModelBuffer();

		return ctx;
	}

	void VulkanRendererAPI::SetCurrentContext(WindowContext *ctx)
	{
		mCurrentContext = ctx;
	}

} // namespace BHive
