#pragma once

#include "VulkanCore.h"
#include "gfx/RendererAPI.h"
#include "gfx/WindowContext.h"
#include "DescriptorPoolManager.h"
#include "ImageState.h"

namespace BHive
{
	class VulkanSwapChain;
	class VulkanFramebuffer;
	class VulkanRendererAPI;
	class Window;
	class VulkanWindowContext;
	class VulkanShader;
	class VulkanPipeline;
	class GPUBuffer;
	class Texture;

	struct FVulkanRendererContext
	{
		FVulkanRendererContext(vk::raii::CommandBuffer &cmd, uint32_t frame, uint32_t imageIndex, uint32_t viewIndex)
			: CommandBuffer(cmd),
			  Frame(frame),
			  ImageIndex(imageIndex),
			  ViewIndex(viewIndex)
		{
		}

		vk::raii::CommandBuffer& CommandBuffer ;

		uint32_t Frame{};

		uint32_t ImageIndex{};

		uint32_t ViewIndex{0};
	};

	
	struct PendingDeletion
	{
		uint32_t Frame = 0;
		std::function<void(uint32_t)> Fn;
	};


	class BHIVE_API VulkanRendererAPI : public RendererAPI
	{
	public:
		VulkanRendererAPI();

		void Init() override;

		void Shutdown() override;

		vk::Result RenderFrame(VulkanSwapChain* swapChain);

		void SubmitGraph(const RenderGraph &graph) override;

		DescriptorPoolManager& GetDescriptorPoolManager() { return mDescriptorPoolManager; }

		void QueueDeletion(FQeueuDeletionFunc&& fn) override;

		void SetCurrentContext(WindowContext *ctx) override;

		WindowContext *GetCurrentContext() const override { return mCurrentContext; }

		void ResetFrameIndex();

		uint32_t GetCurrentFrame() const { return mCurrentFrame; }

	private:
		void ProcessDeletionQueue(uint32_t frame);

		vk::Result ExecuteFinalGraph(VulkanSwapChain* swapChain, RenderGraph &graph);

		void ExecutePass(const FPass &pass, FVulkanRendererContext &ctx, VulkanSwapChain *swapChain);

		void TransitionImages(const FPhase &phase, vk::raii::CommandBuffer &cmd);

		void ExecuteCommandList(const FRenderCommandList &list, FVulkanRendererContext &ctx);

		void BeginSwapChainRendering(const FPhase &phase, FVulkanRendererContext &ctx, VulkanSwapChain *swapChain);

		void BeginOffScreenRendering(const FPass& pass, const FPhase &phase, FVulkanRendererContext &ctx);

		void TransitionSwapChainToPresent(FVulkanRendererContext &ctx, VulkanSwapChain *swapChain);
		
		void EndRendering(const FPhase &phase, FVulkanRendererContext &ctx);

		void CreateBarriers(const FRenderCommandList &list, FVulkanRendererContext &ctx);

		void FlushDeletionQueue();

	private:
		vk::raii::Device& mDevice;

		DescriptorPoolManager mDescriptorPoolManager;

		std::vector<RenderGraph> mSubmittedGraphs;

		std::vector<PendingDeletion> mDeletionQueue;

		uint32_t mCompletedFrame = 0;

		uint32_t mCurrentFrame = 0;

		WindowContext *mCurrentContext = nullptr;

		friend class VulkanFramebuffer;

	};
} // namespace BHive