#pragma once

#include "VulkanCore.h"
#include "gfx/RendererAPI.h"


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
		{}

		vk::raii::CommandBuffer& CommandBuffer;

		uint32_t Frame;

		uint32_t ImageIndex;
	};

	class BHIVE_API VulkanRendererAPI : public RendererAPI
	{

	public:
		VulkanRendererAPI();

		virtual ~VulkanRendererAPI();

		virtual void Init() override;

		virtual void Shutdown() override;

		virtual void WaitIdle() override;

		virtual void ClearColor(float r, float g, float b, float a = 1.0f) override;

		virtual void Clear(ClearMask mask) override;

		virtual void SetLineWidth(float width) override;

		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h) override;

		virtual void DrawArrays(ETopologyMode mode, const Ref<VertexArray> &vao, uint32_t count = 0) override;

		virtual void DrawElements(ETopologyMode mode, const Ref<VertexArray> &vao, uint32_t count = 0) override;

		virtual void DrawElementsBaseVertex(ETopologyMode mode, const Ref<VertexArray> &vao, uint32_t start, uint32_t start_index, uint32_t count = 0, uint32_t instance_count = 0) override;

		virtual void DrawElementsRanged(ETopologyMode mode, const Ref<VertexArray> &vao, uint32_t start, uint32_t end, uint32_t count = 0) override;

		virtual void DrawElementsInstanced(ETopologyMode mode, const Ref<VertexArray> &vao, uint32_t instances, uint32_t count = 0) override;

		virtual void MultiDrawElementsIndirect(ETopologyMode mode, const BufferBase &indirect, const Ref<VertexArray> &vao, size_t drawCount, size_t stride = 0) override;

		virtual void ColorMask(uint8_t r, uint8_t g, uint8_t b, uint8_t a) override;

		virtual void DebugPass(const std::string &msg) override;

		vk::Result RenderFrame(VulkanWindowContext* ctx);

		void SubmitGraph(const RenderGraph &graph, FResourceUpdateList &updateResources) override;

		virtual void SubmitResourceUpdate(FResourceUpdateList::UpdateCommand cmd) override;

		vk::raii::DescriptorPool &GetDescriptorPool() { return mDescriptorPool; }

		virtual EAPI GetAPI() const override { return EAPI::Vulkan; }

		void QueueDeletion(std::function<void(uint32_t)> fn); //frame

		Ref<ISetManager> CreateSetManager(const Pipeline* pipeline, uint32_t setIndex) override;

		void OnPipelineCreated(const VulkanPipeline *pipeline);

		Ref<FComputeBindings> CreateComputeBindings(const Ref<Pipeline> &pipeline);

		AsyncComputeHandle ExecuteComputePass(const Ref<Pipeline> &pipeline, const glm::uvec3 &dispatchSize, const FComputeFunc &builder) override;

	private:
		void ProcessDeletionQueue(uint32_t frame);

		vk::Result ExecuteFinalGraph(VulkanWindowContext *ctx, FResourceUpdateList &updates, const RenderGraph &graph);

		void ExecuteSwapChainPass(const FRenderGraphPass& pass, FVulkanRendererContext& ctx, const Ref<VulkanSwapChain>& swapChain);

		void ExecuteOffScreenPass(const FRenderGraphPass &pass, IRendererContext& ctx);

	public:
		void SetCurrentContext(VulkanWindowContext *ctx);

		VulkanWindowContext *GetCurrentContext() const { return mCurrentContext; }

	private:
		vk::raii::Device &mDevice;

		vk::raii::DescriptorPool mDescriptorPool = nullptr;

		vk::ClearColorValue mClearColor{0, 0, 0, 1};

		std::atomic<bool> mDeviceRecreationInProgress{false};

		VulkanWindowContext *mCurrentContext = nullptr;

		uint32_t  mCompletedFrame = 0;

		std::vector<RenderGraph> mSubmittedGraphs;

		std::vector<FResourceUpdateList> mSubmittedUpdates;

		friend class VulkanFramebuffer;

	};
} // namespace BHive