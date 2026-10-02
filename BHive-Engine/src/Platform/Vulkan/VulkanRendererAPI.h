#pragma once

#include "VulkanBuffer.h"
#include "VulkanCore.h"
#include "VulkanImage.h"
#include "gfx/RendererAPI.h"
#include "gfx/WindowContext.h"

namespace BHive
{
	class VulkanSwapChain;
	class VulkanShader;
	class VulkanPipeline;
	class GeneralBuffer;
	class VulkanBackend;

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

		~VulkanRendererAPI();

		void Init() override;

		void Shutdown() override;

		vk::Result RenderFrame(VulkanSwapChain *swapChain);

		void SubmitGraph(const Graph &graph) override;

		void QueueDeletion(FQeueuDeletionFunc &&fn) override;

		void ResetFrameIndex();

		uint32_t GetCurrentFrame() const { return mCurrentFrame; }

		void CreateTexture2D(int32_t &id, uint32_t x, uint32_t y, const FTextureCreateInfo &info) override;

		void CreateTexture2DArray(int32_t &id, uint32_t x, uint32_t y, const FTextureCreateInfo &info) override;

		void CreateTextureCube(int32_t &id, uint32_t s, const FTextureCreateInfo &info) override;

		void CreateTextureCubeArray(int32_t &id, uint32_t s, const FTextureCreateInfo &info) override;

		void CreateTexture3D(int32_t &id, uint32_t x, uint32_t y, uint32_t z, const FTextureCreateInfo &info) override;

		void DeleteTexture(int32_t &textureID) override;

		void SetTextureData(int32_t textureID, const void *data, size_t size, ImageCopyRegion region, ImageSubresourceRange range) override;

		void CreateBuffer(int32_t &id, EBufferType type, EBufferLifetime lifeTime, size_t size, const char *debugName = nullptr) override;

		void DeleteBuffer(int32_t &bufferID) override;

		void SetBufferData(int32_t bufferID, const void *data, size_t size, uint32_t offset) override;

		void ClearBuffer(int32_t bufferID) override;

		VulkanImage *GetTexture(int32_t textureID);

		VulkanBuffer *GetBuffer(int32_t bufferID);

		static VulkanRendererAPI *GetInstance() { return sInstance; }

	private:
		int32_t CreateTexture(const vk::ImageCreateInfo &imgInfo, const vk::ImageViewCreateInfo &viewInfo, const vk::SamplerCreateInfo &smpInfo, const char *debugName);

		void ProcessDeletionQueue(uint32_t frame);

		vk::Result ExecuteFinalGraph(VulkanSwapChain *swapChain, Graph &graph);

		void ExecutePass(const FPass &pass, FVulkanRendererContext &ctx, VulkanSwapChain *swapChain);

		void TransitionImages(const FPhase &phase, vk::raii::CommandBuffer &cmd);

		void BeginSwapChainRendering(const FPassState &state, const FPhase &phase, FVulkanRendererContext &ctx, VulkanSwapChain *swapChain);

		void BeginOffScreenRendering(const FPassState &state, const FPhase &phase, FVulkanRendererContext &ctx);

		void EndSwapChainRendering(FVulkanRendererContext &ctx, VulkanSwapChain *swapChain);

		void EndOffScreenRendering(const FPhase &phase, FVulkanRendererContext &ctx);

		void FlushDeletionQueue();

		FVulkanRendererContext BuildContext(vk::raii::CommandBuffer &cmd, uint32_t frame, uint32_t imageIndex, uint32_t viewIndex);

	private:
		Ref<VulkanBackend> mBackend;

		std::vector<Graph> mSubmittedGraphs;

		std::vector<PendingDeletion> mDeletionQueue;

		uint32_t mCompletedFrame = 0;

		uint32_t mCurrentFrame = 0;

		static VulkanRendererAPI *sInstance;

		friend class VulkanFramebuffer;

		std::deque<VulkanImage> mTextures;
		std::queue<uint32_t> mTextureFreeList;
		std::deque<VulkanBuffer> mBuffers;
		std::queue<uint32_t> mBufferFreeList;
	};
} // namespace BHive