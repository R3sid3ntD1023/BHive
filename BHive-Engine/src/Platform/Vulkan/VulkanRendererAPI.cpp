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
		FlushDeletionQueue();

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

	void VulkanRendererAPI::CreateTexture2D(int32_t &id, uint32_t x, uint32_t y, const FTextureCreateInfo &info)
	{
		auto format = ToVkFormat(info.Format);
		auto levels = info.MipLevels;
		auto layers = info.ArrayLayers;
		auto extent = vk::Extent3D(x, y, 1);
		auto usage = InferImageUsage(info.Roles);
		auto aspect = ToVkAspect(info.Aspect);
		auto magFilter = ToVkFilter(info.MagFilter);
		auto minFilter = ToVkFilter(info.MinFilter);
		auto addressMode = ToVkWrap(info.WrapMode);
		auto compare_enabled = info.CompareOp.has_value();
		auto compare_op = compare_enabled ? ToVkCompare(info.CompareOp.value()) : vk::CompareOp::eAlways;
		auto range = vk::ImageSubresourceRange(aspect, 0, levels, 0, layers);

		vk::ImageCreateInfo imgInfo(
			{}, vk::ImageType::e2D, format, extent, levels, layers, vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal, usage, vk::SharingMode::eExclusive, 0
		);

		vk::ImageViewCreateInfo viewInfo({}, VK_NULL_HANDLE, vk::ImageViewType::e2D, format, {}, range);

		vk::SamplerCreateInfo smpInfo(
			{},
			magFilter,
			minFilter,
			vk::SamplerMipmapMode::eLinear,
			addressMode,
			addressMode,
			addressMode,
			0.0f,
			0u,
			1.0f,
			compare_enabled,
			compare_op,
			0.0f,
			float(levels - 1),
			ToVkBorderColor(info.BorderColor),
			VK_FALSE
		);

		id = CreateTexture(imgInfo, viewInfo, smpInfo, info.DebugName.c_str());
	}

	void VulkanRendererAPI::CreateTexture2DArray(int32_t &id, uint32_t x, uint32_t y, const FTextureCreateInfo &info)
	{
		auto format = ToVkFormat(info.Format);
		auto levels = info.MipLevels;
		auto layers = info.ArrayLayers;
		auto extent = vk::Extent3D(x, y, 1);
		auto usage = InferImageUsage(info.Roles);
		auto aspect = ToVkAspect(info.Aspect);
		auto magFilter = ToVkFilter(info.MagFilter);
		auto minFilter = ToVkFilter(info.MinFilter);
		auto addressMode = ToVkWrap(info.WrapMode);
		auto compare_enabled = info.CompareOp.has_value();
		auto compare_op = compare_enabled ? ToVkCompare(info.CompareOp.value()) : vk::CompareOp::eAlways;
		auto range = vk::ImageSubresourceRange(aspect, 0, levels, 0, layers);

		vk::ImageCreateInfo imgInfo(
			{}, vk::ImageType::e2D, format, extent, levels, layers, vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal, usage, vk::SharingMode::eExclusive, 0
		);

		vk::ImageViewCreateInfo viewInfo({}, VK_NULL_HANDLE, vk::ImageViewType::e2DArray, format, {}, range);

		vk::SamplerCreateInfo smpInfo(
			{},
			magFilter,
			minFilter,
			vk::SamplerMipmapMode::eLinear,
			addressMode,
			addressMode,
			addressMode,
			0.0f,
			0u,
			1.0f,
			compare_enabled,
			compare_op,
			0.0f,
			float(levels - 1),
			ToVkBorderColor(info.BorderColor),
			VK_FALSE
		);

		id = CreateTexture(imgInfo, viewInfo, smpInfo, info.DebugName.c_str());
	}

	void VulkanRendererAPI::CreateTextureCube(int32_t &id, uint32_t s, const FTextureCreateInfo &info)
	{
		auto format = ToVkFormat(info.Format);
		auto levels = info.MipLevels;
		auto layers = 6;
		auto extent = vk::Extent3D(s, s, 1);
		auto usage = InferImageUsage(info.Roles);
		auto aspect = ToVkAspect(info.Aspect);
		auto magFilter = ToVkFilter(info.MagFilter);
		auto minFilter = ToVkFilter(info.MinFilter);
		auto addressMode = ToVkWrap(info.WrapMode);
		auto compare_enabled = info.CompareOp.has_value();
		auto compare_op = compare_enabled ? ToVkCompare(info.CompareOp.value()) : vk::CompareOp::eAlways;

		vk::ImageCreateInfo imgInfo(
			vk::ImageCreateFlagBits::eCubeCompatible,
			vk::ImageType::e2D,
			format,
			extent,
			levels,
			layers,
			vk::SampleCountFlagBits::e1,
			vk::ImageTiling::eOptimal,
			usage,
			vk::SharingMode::eExclusive,
			0
		);

		auto range = vk::ImageSubresourceRange(aspect, 0, levels, 0, layers);
		vk::ImageViewCreateInfo viewInfo({}, VK_NULL_HANDLE, vk::ImageViewType::eCube, format, {}, range);

		vk::SamplerCreateInfo smpInfo(
			{},
			magFilter,
			minFilter,
			vk::SamplerMipmapMode::eLinear,
			addressMode,
			addressMode,
			addressMode,
			0.0f,
			0u,
			1.0f,
			compare_enabled,
			compare_op,
			0.0f,
			float(levels - 1),
			ToVkBorderColor(info.BorderColor),
			VK_FALSE
		);

		id = CreateTexture(imgInfo, viewInfo, smpInfo, info.DebugName.c_str());
	}

	void VulkanRendererAPI::CreateTextureCubeArray(int32_t &id, uint32_t s, const FTextureCreateInfo &info)
	{
		auto layers = info.ArrayLayers * 6;
		auto format = ToVkFormat(info.Format);
		auto levels = info.MipLevels;
		auto extent = vk::Extent3D(s, s, 1);
		auto usage = InferImageUsage(info.Roles);
		auto aspect = ToVkAspect(info.Aspect);
		auto range = vk::ImageSubresourceRange(aspect, 0, levels, 0, layers);
		auto magFilter = ToVkFilter(info.MagFilter);
		auto minFilter = ToVkFilter(info.MinFilter);
		auto addressMode = ToVkWrap(info.WrapMode);
		auto compare_enabled = info.CompareOp.has_value();
		auto compare_op = compare_enabled ? ToVkCompare(info.CompareOp.value()) : vk::CompareOp::eAlways;

		vk::ImageCreateInfo imgInfo(
			vk::ImageCreateFlagBits::eCubeCompatible,
			vk::ImageType::e2D,
			format,
			extent,
			levels,
			layers,
			vk::SampleCountFlagBits::e1,
			vk::ImageTiling::eOptimal,
			usage,
			vk::SharingMode::eExclusive,
			0
		);

		vk::ImageViewCreateInfo viewInfo({}, VK_NULL_HANDLE, vk::ImageViewType::eCubeArray, format, {}, range);

		vk::SamplerCreateInfo smpInfo(
			{},
			magFilter,
			minFilter,
			vk::SamplerMipmapMode::eLinear,
			addressMode,
			addressMode,
			addressMode,
			0.0f,
			0u,
			1.0f,
			compare_enabled,
			compare_op,
			0.0f,
			float(levels - 1),
			ToVkBorderColor(info.BorderColor),
			VK_FALSE
		);
		id = CreateTexture(imgInfo, viewInfo, smpInfo, info.DebugName.c_str());
	}

	void VulkanRendererAPI::CreateTexture3D(int32_t &id, uint32_t x, uint32_t y, uint32_t z, const FTextureCreateInfo &info)
	{
		auto format = ToVkFormat(info.Format);
		auto levels = info.MipLevels;
		auto layers = info.ArrayLayers;
		auto extent = vk::Extent3D(x, y, z);
		auto usage = InferImageUsage(info.Roles);
		auto aspect = ToVkAspect(info.Aspect);
		auto range = vk::ImageSubresourceRange(aspect, 0, levels, 0, layers);
		auto magFilter = ToVkFilter(info.MagFilter);
		auto minFilter = ToVkFilter(info.MinFilter);
		auto addressMode = ToVkWrap(info.WrapMode);
		auto compare_enabled = info.CompareOp.has_value();
		auto compare_op = compare_enabled ? ToVkCompare(info.CompareOp.value()) : vk::CompareOp::eAlways;

		vk::ImageCreateInfo imgInfo(
			{}, vk::ImageType::e3D, format, extent, levels, layers, vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal, usage, vk::SharingMode::eExclusive, 0
		);

		vk::ImageViewCreateInfo viewInfo({}, VK_NULL_HANDLE, vk::ImageViewType::e3D, format, {}, range);

		vk::SamplerCreateInfo smpInfo(
			{},
			magFilter,
			minFilter,
			vk::SamplerMipmapMode::eLinear,
			addressMode,
			addressMode,
			addressMode,
			0.0f,
			0u,
			1.0f,
			compare_enabled,
			compare_op,
			0.0f,
			float(levels - 1),
			ToVkBorderColor(info.BorderColor),
			VK_FALSE
		);
		id = CreateTexture(imgInfo, viewInfo, smpInfo, info.DebugName.c_str());
	}

	void VulkanRendererAPI::DeleteTexture(int32_t &textureID)
	{
		if (textureID != -1 || textureID >= mTextures.size())
		{
			return;
		}

		mTextures[textureID] = VulkanImage();
		mTextureFreeList.push(textureID);
		textureID = -1;
	}

	void VulkanRendererAPI::SetTextureData(int32_t textureID, const void *data, size_t size, ImageCopyRegion region, ImageSubresourceRange range)
	{
		if (textureID >= mTextures.size() || textureID == -1)
			return;

		mTextures[textureID].Upload(data, size, region, range);
	}

	VulkanImage *VulkanRendererAPI::GetTexture(int32_t textureID)
	{
		if (textureID == -1 || textureID >= static_cast<int32_t>(mTextures.size()))
			return nullptr;
		return &mTextures.at(textureID);
	}

	int32_t
	VulkanRendererAPI::CreateTexture(const vk::ImageCreateInfo &imgInfo, const vk::ImageViewCreateInfo &viewInfo, const vk::SamplerCreateInfo &smpInfo, const char *debugName)
	{
		int32_t textureID = -1;

		if (!mTextureFreeList.empty())
		{
			auto textureID = mTextureFreeList.front();
			mTextureFreeList.pop();
			auto &image = mTextures.at(textureID);
			image.Initialize(imgInfo, viewInfo, smpInfo);
			image.SetDebugName(debugName);
			return textureID;
		}

		auto &image = mTextures.emplace_back();
		image.Initialize(imgInfo, viewInfo, smpInfo);
		image.SetDebugName(debugName);

		textureID = static_cast<uint32_t>(mTextures.size() - 1);
		return textureID;
	}

	void VulkanRendererAPI::CreateBuffer(int32_t &id, EBufferType type, EBufferLifetime lifeTime, size_t size, const char *debugName)
	{
		if (!mBufferFreeList.empty())
		{
			id = mBufferFreeList.front();
			mBufferFreeList.pop();
			auto &buffer = mBuffers.at(id);
			buffer.Initialize(size, ToVkBufferType(type), lifeTime);
			return;
		}

		auto &buffer = mBuffers.emplace_back();
		buffer.Initialize(size, ToVkBufferType(type), lifeTime);
		id = static_cast<uint32_t>(mBuffers.size() - 1);
	}

	void VulkanRendererAPI::DeleteBuffer(int32_t &bufferID)
	{
		if (bufferID != -1 && bufferID < static_cast<int32_t>(mBuffers.size()))
		{
			mBuffers[bufferID] = VulkanBuffer();
			mBufferFreeList.push(bufferID);
			bufferID = -1;
		}
	}

	void VulkanRendererAPI::SetBufferData(int32_t bufferID, const void *data, size_t size, uint32_t offset)
	{
		if (!data || size == 0)
			return;

		if (bufferID == -1 || bufferID >= static_cast<int32_t>(mBuffers.size()))
			return;

		mBuffers[bufferID].Upload(data, size, offset);
	}

	void VulkanRendererAPI::ClearBuffer(int32_t bufferID)
	{
		if (bufferID == -1 || bufferID >= static_cast<int32_t>(mBuffers.size()))
			return;

		mBuffers[bufferID].ClearData();
	}

	VulkanBuffer *VulkanRendererAPI::GetBuffer(int32_t bufferID)
	{
		if (bufferID == -1 || bufferID >= static_cast<int32_t>(mBuffers.size()))
			return nullptr;
		return &mBuffers.at(bufferID);
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
