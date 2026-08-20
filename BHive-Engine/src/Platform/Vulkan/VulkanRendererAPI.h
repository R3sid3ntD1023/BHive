#pragma once

#include "VulkanCore.h"
#include "gfx/RendererAPI.h"
#include "gfx/WindowContext.h"
#include "DescriptorPoolManager.h"
#include "ImageState.h"

namespace BHive
{
	class VulkanSwapChain;
	class VulkanShader;
	class VulkanPipeline;
	class GeneralBuffer;

	struct FVulkanRendererContext
	{
		FVulkanRendererContext(vk::raii::CommandBuffer &cmd, uint32_t frame, uint32_t imageIndex, uint32_t viewIndex)
			: CommandBuffer(cmd),
			  Frame(frame),
			  ImageIndex(imageIndex),
			  ViewIndex(viewIndex)
		{
		}

		vk::raii::CommandBuffer &CommandBuffer;

		uint32_t Frame{};

		uint32_t ImageIndex{};

		uint32_t ViewIndex{0};

		Ref<GeneralBuffer> ModelBuffer;
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

		vk::Result RenderFrame(VulkanSwapChain *swapChain);

		void SubmitGraph(const RenderGraph &graph) override;

		DescriptorPoolManager &GetDescriptorPoolManager() { return mDescriptorPoolManager; }

		void QueueDeletion(FQeueuDeletionFunc &&fn) override;

		void SetCurrentContext(WindowContext *ctx) override;

		WindowContext *GetCurrentContext() const override { return mCurrentContext; }

		void ResetFrameIndex();

		uint32_t GetCurrentFrame() const { return mCurrentFrame; }

	private:
		void ProcessDeletionQueue(uint32_t frame);

		vk::Result ExecuteFinalGraph(VulkanSwapChain *swapChain, RenderGraph &graph);

		void ExecutePass(const FPass &pass, FVulkanRendererContext &ctx, VulkanSwapChain *swapChain);

		void TransitionImages(const FPhase &phase, vk::raii::CommandBuffer &cmd);

		void BeginSwapChainRendering(const FPassState &state, const FPhase &phase, FVulkanRendererContext &ctx, VulkanSwapChain *swapChain);

		void BeginOffScreenRendering(const FPassState &state, const FPhase &phase, FVulkanRendererContext &ctx);

		void TransitionSwapChainToPresent(FVulkanRendererContext &ctx, VulkanSwapChain *swapChain);

		void EndRendering(FVulkanRendererContext &ctx);

		void CreateBarriers(const FRenderCommandList &list, FVulkanRendererContext &ctx);

		void FlushDeletionQueue();

		FVulkanRendererContext BuildContext(vk::raii::CommandBuffer &cmd, uint32_t frame, uint32_t imageIndex, uint32_t viewIndex);

	private:
		vk::raii::Device &mDevice;

		DescriptorPoolManager mDescriptorPoolManager;

		std::vector<RenderGraph> mSubmittedGraphs;

		std::vector<PendingDeletion> mDeletionQueue;

		uint32_t mCompletedFrame = 0;

		uint32_t mCurrentFrame = 0;

		WindowContext *mCurrentContext = nullptr;

		friend class VulkanFramebuffer;
	};
} // namespace BHive