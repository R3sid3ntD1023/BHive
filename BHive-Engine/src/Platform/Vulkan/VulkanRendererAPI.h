#pragma once

#include "VulkanCore.h"
#include "gfx/RendererAPI.h"
#include "gfx/WindowContext.h"
#include "DescriptorPoolManager.h"

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

	struct FVulkanRendererContext : public IRendererContext
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

	struct FVulkanTransferContext : public ITransferContext
	{
		FVulkanTransferContext(vk::raii::CommandBuffer& cmd)
			: Cmd(cmd)
		{}

		vk::raii::CommandBuffer& Cmd;
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

		void WaitIdle() override;

		void ClearColor(FRenderGraphPass *pass, float r, float g, float b, float a = 1.0f) override;

		void Clear(FRenderGraphPass *pass, ClearMask mask) override;

		void SetLineWidth(FRenderGraphPass *pass, float width) override;

		void SetViewport(FRenderGraphPass *pass, uint32_t x, uint32_t y, uint32_t w, uint32_t h) override;

		void DrawArrays(FRenderGraphPass *pass, ETopologyMode mode, VertexArray* vao, uint32_t count = 0) override;

		void DrawElements(FRenderGraphPass *pass, ETopologyMode mode, VertexArray* vao, uint32_t count = 0) override;

		void
		DrawElementsBaseVertex(FRenderGraphPass *pass, ETopologyMode mode, VertexArray* vao, uint32_t start, uint32_t start_index, uint32_t count = 0, uint32_t instance_count = 0) override;

		void DrawElementsRanged(FRenderGraphPass *pass, ETopologyMode mode, VertexArray* vao, uint32_t start, uint32_t end, uint32_t count = 0) override;

		void DrawElementsInstanced(FRenderGraphPass *pass, ETopologyMode mode, VertexArray* vao, uint32_t instances, uint32_t count = 0) override;

		void MultiDrawElementsIndirect(FRenderGraphPass *pass, ETopologyMode mode, BufferBase* indirect, VertexArray* vao, uint32_t drawCount, uint32_t stride = 0, uint32_t offset = 0) override;

		void ColorMask(FRenderGraphPass *pass, uint8_t r, uint8_t g, uint8_t b, uint8_t a) override;

		vk::Result RenderFrame(VulkanSwapChain* swapChain);

		void SubmitGraph(const RenderGraph &graph, FResourceUpdateList &updateResources) override;

		DescriptorPoolManager& GetDescriptorPoolManager() { return mDescriptorPoolManager; }

		void QueueDeletion(FQeueuDeflectionFunc&& fn) override;

		Ref<FComputeBindings> CreateComputeBindings(Pipeline* pipeline) override;

		FAsyncPass* ExecuteComputePass(Pipeline* pipeline, const glm::uvec3 &dispatchSize, const FComputeFunc &builder) override;

		void ExecuteTransferPass(FTransferFunc &&builder) override;

		void SetCurrentContext(WindowContext *ctx) override;

		WindowContext *GetCurrentContext() const override { return mCurrentContext; }

		void ResetFrameIndex();

		uint32_t GetCurrentFrame() const { return mCurrentFrame; }

	private:
		void ProcessDeletionQueue(uint32_t frame);

		vk::Result ExecuteFinalGraph(VulkanSwapChain* swapChain, FResourceUpdateList &updates, const RenderGraph &graph);

		void ExecuteSwapChainPass(const FRenderGraphPass& pass, FVulkanRendererContext& ctx, VulkanSwapChain* swapChain);

		void ExecuteOffScreenPass(const FRenderGraphPass &pass, FVulkanRendererContext& ctx);

		void ExecutePass(const FRenderGraphPass &pass, FVulkanRendererContext &ctx, VulkanSwapChain *swapChain);
	private:
		vk::raii::Device& mDevice;

		DescriptorPoolManager mDescriptorPoolManager;

		vk::ClearColorValue mClearColor{0, 0, 0, 1};

		std::atomic<bool> mDeviceRecreationInProgress{false};

		WindowContext *mCurrentContext = nullptr;

		std::vector<RenderGraph> mSubmittedGraphs;

		std::vector<FResourceUpdateList> mSubmittedUpdates;

		std::vector<Scope<FAsyncPass>> mComputePasses;

		std::queue<PendingDeletion> mDeletionQueue;

		uint32_t mCompletedFrame = 0;

		uint32_t mCurrentFrame = 0;

		friend class VulkanFramebuffer;

	};
} // namespace BHive