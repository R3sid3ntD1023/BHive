#pragma once

#include "core/Core.h"
#include "Enumerations.h"
#include "RenderGraph.h"

namespace BHive
{
	class VertexArray;
	class BufferBase;
	class Pipeline;
	class WindowContext;


	struct MultiDrawIndirectCommand
	{
		uint32_t Count;
		uint32_t InstanceCount;
		uint32_t FirstIndex;
		int32_t BaseVertex;
		uint32_t BaseInstance;
	};

	struct BHIVE_API ITransferContext
	{
		virtual ~ITransferContext() = default;

		template<typename TContext>
		requires(std::is_base_of_v<ITransferContext, TContext>)
		TContext& As()
		{
			ASSERT(dynamic_cast<TContext *>(this) != nullptr);
			return static_cast<TContext &>(*this);
		}
	};

	using FComputeFunc = std::function<void(FComputeBindings &)>;
	using FQeueuDeflectionFunc = std::function<void(uint32_t)>;
	using FTransferFunc = std::function<void(ITransferContext &)>;


	class BHIVE_API RendererAPI
	{
	public:
		enum EAPI
		{
			Opengl,
			Vulkan
		};

	public:
		virtual ~RendererAPI() = default;

		virtual void Init() = 0;

		virtual void Shutdown() = 0;

		virtual void WaitIdle() = 0;

		virtual void ClearColor(FRenderGraphPass *pass, float r, float g, float b, float a = 1.0f) = 0;

		virtual void Clear(FRenderGraphPass *pass, ClearMask mask = ClearMask::All) = 0;

		virtual void SetLineWidth(FRenderGraphPass *pass, float width) = 0;

		virtual void SetViewport(FRenderGraphPass* pass, uint32_t x, uint32_t y, uint32_t w, uint32_t h) = 0;

		virtual void DrawArrays(FRenderGraphPass *pass, ETopologyMode mode, VertexArray* vao, uint32_t count) = 0;

		virtual void DrawElements(FRenderGraphPass *pass, ETopologyMode mode, VertexArray* vao, uint32_t count) = 0;

		virtual void
		DrawElementsBaseVertex(FRenderGraphPass *pass, ETopologyMode mode, VertexArray* vao, uint32_t start, uint32_t start_index, uint32_t count , uint32_t instance_count) = 0;

		virtual void DrawElementsRanged(FRenderGraphPass *pass, ETopologyMode mode, VertexArray* vao, uint32_t start, uint32_t end, uint32_t count ) = 0;

		virtual void DrawElementsInstanced(FRenderGraphPass *pass, ETopologyMode mode, VertexArray* vao, uint32_t instances, uint32_t count) = 0;

		virtual void MultiDrawElementsIndirect(FRenderGraphPass *pass, ETopologyMode mode, BufferBase* indirect, VertexArray* vao, uint32_t drawCount, uint32_t stride, uint32_t offset ) = 0;

		virtual void ColorMask(FRenderGraphPass *pass, uint8_t r, uint8_t g, uint8_t b, uint8_t a) = 0;

		virtual void SubmitGraph(const RenderGraph &graph, FResourceUpdateList &updateResources) = 0;

		virtual Ref<FComputeBindings> CreateComputeBindings(Pipeline* pipeline) = 0;

		virtual FAsyncPass* ExecuteComputePass(Pipeline* pipeline, const glm::uvec3 &dispatchSize, const FComputeFunc &builder) = 0;

		virtual void ExecuteTransferPass(FTransferFunc && builder) = 0;

		virtual void QueueDeletion(FQeueuDeflectionFunc &&fn) = 0;

		virtual void SetCurrentContext(WindowContext *ctx) = 0;

		virtual WindowContext *GetCurrentContext() const = 0;

		static Scope<RendererAPI> Create();
	};
} // namespace BHive