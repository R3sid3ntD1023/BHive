#pragma once

#include "VulkanCore.h"
#include "gfx/RendererAPI.h"
#include "gfx/WindowContext.h"

namespace BHive
{
	class VulkanSwapChain;
	class VulkanFramebuffer;
	class VulkanRendererAPI;
	class Window;
	class VulkanWindowContext;
	class VulkanShader;
	class VulkanPipeline;

	struct FVulkanRendererContext : public IRendererContext
	{
		FVulkanRendererContext(vk::raii::CommandBuffer& cmd, uint32_t frame, uint32_t imageIndex)
			: CommandBuffer(cmd),
			  Frame(frame),
			  ImageIndex(imageIndex)
		{
		}

		vk::raii::CommandBuffer& CommandBuffer ;

		uint32_t Frame{};

		uint32_t ImageIndex{};
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

		virtual void Init() override;

		virtual void Shutdown() override;

		virtual void WaitIdle() override;

		virtual void ClearColor(FRenderGraphPass *pass, float r, float g, float b, float a = 1.0f) override;

		virtual void Clear(FRenderGraphPass *pass, ClearMask mask) override;

		virtual void SetLineWidth(FRenderGraphPass *pass, float width) override;

		virtual void SetViewport(FRenderGraphPass *pass, uint32_t x, uint32_t y, uint32_t w, uint32_t h) override;

		virtual void DrawArrays(FRenderGraphPass *pass, ETopologyMode mode, VertexArray* vao, uint32_t count = 0) override;

		virtual void DrawElements(FRenderGraphPass *pass, ETopologyMode mode, VertexArray* vao, uint32_t count = 0) override;

		virtual void
		DrawElementsBaseVertex(FRenderGraphPass *pass, ETopologyMode mode, VertexArray* vao, uint32_t start, uint32_t start_index, uint32_t count = 0, uint32_t instance_count = 0) override;

		virtual void DrawElementsRanged(FRenderGraphPass *pass, ETopologyMode mode, VertexArray* vao, uint32_t start, uint32_t end, uint32_t count = 0) override;

		virtual void DrawElementsInstanced(FRenderGraphPass *pass, ETopologyMode mode, VertexArray* vao, uint32_t instances, uint32_t count = 0) override;

		virtual void MultiDrawElementsIndirect(FRenderGraphPass *pass, ETopologyMode mode, BufferBase* indirect, VertexArray* vao, size_t drawCount, size_t stride = 0) override;

		virtual void ColorMask(FRenderGraphPass *pass, uint8_t r, uint8_t g, uint8_t b, uint8_t a) override;

		vk::Result RenderFrame(VulkanSwapChain* swapChain);

		void SubmitGraph(const RenderGraph &graph, FResourceUpdateList &updateResources) override;

		vk::raii::DescriptorPool &GetDescriptorPool() { return mDescriptorPool; }

		void QueueDeletion(FQeueuDeflectionFunc&& fn) override;

		Ref<ISetManager> CreateSetManager(const Pipeline* pipeline, uint32_t setIndex) override;

		void OnPipelineCreated(const VulkanPipeline *pipeline);

		Ref<FComputeBindings> CreateComputeBindings(Pipeline* pipeline) override;

		FAsyncPass* ExecuteComputePass(Pipeline* pipeline, const glm::uvec3 &dispatchSize, const FComputeFunc &builder) override;

		virtual void ExecuteTransferPass(FTransferFunc &&builder) override;

		void SetCurrentContext(WindowContext *ctx) override;

		WindowContext *GetCurrentContext() const override { return mCurrentContext; }

		void ResetFrameIndex();

	private:
		void ProcessDeletionQueue(uint32_t frame);

		vk::Result ExecuteFinalGraph(VulkanSwapChain* swapChain, FResourceUpdateList &updates, const RenderGraph &graph);

		void ExecuteSwapChainPass(const FRenderGraphPass& pass, FVulkanRendererContext& ctx, VulkanSwapChain* swapChain);

		void ExecuteOffScreenPass(const FRenderGraphPass &pass, IRendererContext& ctx);

	private:
		vk::raii::Device& mDevice;

		vk::raii::DescriptorPool mDescriptorPool = nullptr;

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