#include "VulkanRendererAPI.h"
#include "VulkanBackend.h"
#include "VulkanConversions.h"
#include "VulkanFramebuffer.h"
#include "VulkanInterpreter.h"
#include "VulkanQuery.h"
#include "VulkanSwapChain.h"
#include "gfx/renderers/Renderer.h"

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

	VulkanRendererAPI *VulkanRendererAPI::sInstance = nullptr;

	VulkanRendererAPI::VulkanRendererAPI()
	{
		sInstance = this;
	}

	VulkanRendererAPI::~VulkanRendererAPI()
	{
		sInstance = nullptr;
	}

	void VulkanRendererAPI::Init()
	{
		mBackend = CreateScope<VulkanBackend>();
		mBackend->Init();
	}

	void VulkanRendererAPI::Shutdown()
	{
		VulkanBackend::GetLogicalDevice().waitIdle();
		FlushDeletionQueue();
		mTextures.ProcessDeletions();
		mBuffers.ProcessDeletions();
		mTextures.Clear();
		mBuffers.Clear();

		mBackend->Shutdown();
	}

	vk::Result VulkanRendererAPI::RenderFrame(VulkanSwapChain *swapChain)
	{

		Graph finalGraph;
		for (auto &g : mSubmittedGraphs)
			finalGraph.Append(g);

		mSubmittedGraphs.clear();

		if (finalGraph.IsEmpty())
		{
			return vk::Result::eSuccess;
		}

		return ExecuteFinalGraph(swapChain, finalGraph);
	}

	void VulkanRendererAPI::SubmitGraph(const Graph &graph)
	{
		if (!graph.IsEmpty())
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
	}

	void VulkanRendererAPI::CreateTexture(int32_t &id, ETextureType type, const glm::uvec3 &size, const FTextureCreateInfo &info)
	{
		FVulkanTextureCreateInfo createInfo(type, size, info);
		id = CreateTexture(createInfo.ImageInfo, createInfo.ViewInfo, createInfo.SamplerInfo, info.DebugName.c_str());
	}

	void VulkanRendererAPI::DeleteTexture(int32_t &textureID)
	{
		mTextures.Delete(textureID);
	}

	void VulkanRendererAPI::SetTextureData(int32_t textureID, const void *data, size_t size, ImageCopyRegion region, ImageSubresourceRange range)
	{
		if (auto texture = GetTexture(textureID))
			texture->Upload(data, size, region, range);
	}

	VulkanImage *VulkanRendererAPI::GetTexture(int32_t textureID)
	{
		return mTextures.Get(textureID);
	}

	int32_t
	VulkanRendererAPI::CreateTexture(const vk::ImageCreateInfo &imgInfo, const vk::ImageViewCreateInfo &viewInfo, const vk::SamplerCreateInfo &smpInfo, const char *debugName)
	{
		auto textureID = mTextures.Create();
		auto image = GetTexture(textureID);
		image->Initialize(imgInfo, viewInfo, smpInfo);
		image->SetDebugName(debugName);
		return textureID;
	}

	void VulkanRendererAPI::CreateBuffer(int32_t &id, EBufferType type, EBufferLifetime lifeTime, size_t size, const char *debugName)
	{
		id = mBuffers.Create(VulkanBackend::GetLogicalDevice());
		mBuffers.Get(id)->Initialize(size, ToVkBufferType(type), lifeTime);
	}

	void VulkanRendererAPI::DeleteBuffer(int32_t &bufferID)
	{
		mBuffers.Delete(bufferID);
	}

	void VulkanRendererAPI::SetBufferData(int32_t bufferID, const void *data, size_t size, uint32_t offset)
	{
		if (!data || size == 0)
			return;

		if (auto buffer = GetBuffer(bufferID))
			buffer->Upload(data, size, offset);
	}

	void VulkanRendererAPI::ClearBuffer(int32_t bufferID)
	{
		if (auto buffer = GetBuffer(bufferID))
			buffer->ClearData();
	}

	VulkanBuffer *VulkanRendererAPI::GetBuffer(int32_t bufferID)
	{
		return mBuffers.Get(bufferID);
	}

	void VulkanRendererAPI::ProcessDeletionQueue(uint32_t frame)
	{
		if (mTextures.HasPendingDeletions() || mBuffers.HasPendingDeletions())
			VulkanBackend::GetLogicalDevice().waitIdle();

		mTextures.ProcessDeletions();
		mBuffers.ProcessDeletions();

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

	vk::Result VulkanRendererAPI::ExecuteFinalGraph(VulkanSwapChain *swapChain, Graph &graph)
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
			pass.ResolveBufferTransitons();

			ExecutePass(pass, vk_ctx, swapChain);
		}

		cmd.end();

		swapChain->ResetFence(current_frame);

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

		const bool passLabel = EngineConfig::DebugLabels && !pass.Name.empty();
		if (passLabel)
		{
			vk::DebugUtilsLabelEXT label(pass.Name.c_str(), {1.0f, .5f, 0.0f, 1.0f});
			cmd.beginDebugUtilsLabelEXT(label);
		}

		for (auto &phase : pass.Phases)
		{
			const bool phaseLabel = EngineConfig::DebugPhaseLabels && !phase.Name.empty();
			if (phaseLabel)
			{
				vk::DebugUtilsLabelEXT label(phase.Name.c_str(), {1.0f, 0.0f, 1.0f, 1.0f});
				cmd.beginDebugUtilsLabelEXT(label);
			}

			VulkanInterpreter::CreateBarriers(phase.BufferTransitions, ctx);

			TransitionImages(phase, cmd);

			if (phase.Type == EPhaseType::Graphics)
			{
				if (pass.Type == EPassType::Present)
					BeginSwapChainRendering(state, phase, ctx, swapChain);
				else
					BeginOffScreenRendering(state, phase, ctx);
			}

			VulkanInterpreter::ExecuteCommandList(phase, ctx);

			if (phase.Type == EPhaseType::Graphics)
			{
				if (pass.Type == EPassType::Present)
					EndSwapChainRendering(ctx, swapChain);
				else
				{
					EndOffScreenRendering(phase, ctx);
				}
			}

			if (phaseLabel)
				cmd.endDebugUtilsLabelEXT();
		}

		if (passLabel)
			cmd.endDebugUtilsLabelEXT();
	}

	void VulkanRendererAPI::TransitionImages(const FPhase &phase, vk::raii::CommandBuffer &cmd)
	{
		for (auto &imgInfo : phase.Images)
		{
			if (!imgInfo.Texture)
				continue;

			auto tex = imgInfo.Texture.As<Texture>();
			auto textureID = tex->GetTextureID();

			if (textureID == -1)
			{
				continue;
			}

			auto vkImage = GetTexture(textureID);

			ImageState oldState = vkImage->GetState(imgInfo.Range.BaseMipLevel, imgInfo.Range.BaseArrayLayer);
			ImageState newState = ImageState::ToImageState(imgInfo.Access);

			if (oldState.IsUndefined || oldState != newState)
			{
				vkImage->Transition(cmd, newState, imgInfo.Range);
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
		auto fbo = phase.BoundFBO.FBO;
		if (fbo)
		{
			const auto framebuffer = fbo.As<VulkanFramebuffer>();
			const auto range = phase.BoundFBO.Range;
			auto &cmd = ctx.CommandBuffer;

			VulkanFramebuffer::RenderInfo renderInfo;
			renderInfo.ClearColor = vk::ClearColorValue(state.Color.ClearColor.r, state.Color.ClearColor.g, state.Color.ClearColor.b, state.Color.ClearColor.a);
			renderInfo.ClearDepthValue = vk::ClearDepthStencilValue(1.0f, 0);
			renderInfo.ColorLoadOp = utils::ToLoad(state.Color.LoadOP);
			renderInfo.ColorStoreOp = utils::ToStore(state.Color.StoreOP);
			renderInfo.DepthLoadOp = utils::ToLoad(state.Depth.LoadOP);
			renderInfo.DepthStoreOp = utils::ToStore(state.Depth.StoreOP);

			framebuffer->BeginRendering(cmd, range, renderInfo);
		}
	}

	void VulkanRendererAPI::EndSwapChainRendering(FVulkanRendererContext &ctx, VulkanSwapChain *swapChain)
	{
		swapChain->EndRendering(ctx.CommandBuffer, ctx.ImageIndex);
	}

	void VulkanRendererAPI::EndOffScreenRendering(const FPhase &phase, FVulkanRendererContext &ctx)
	{
		auto fbo = phase.BoundFBO.FBO;
		if (fbo)
		{
			const auto framebuffer = fbo.As<VulkanFramebuffer>();
			const auto range = phase.BoundFBO.Range;
			auto &cmd = ctx.CommandBuffer;

			framebuffer->EndRendering(cmd, range);
		}
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
