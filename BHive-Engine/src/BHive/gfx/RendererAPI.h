#pragma once

#include "core/Core.h"
#include "Enumerations.h"
#include "RenderGraph.h"
#include "ISetManager.h"

namespace BHive
{
	class VertexArray;
	class BufferBase;
	class Pipeline;

	using FComputeFunc = std::function<void(FComputeBindings &)>;

	struct MultiDrawIndirectCommand
	{
		uint32_t Count;
		uint32_t InstanceCount;
		uint32_t FirstIndex;
		int32_t BaseVertex;
		uint32_t BaseInstance;
	};

	struct BHIVE_API AsyncComputeHandle
	{
		void* Fence = nullptr;
		void *CommandBuffer = nullptr;
		Ref<FComputeBindings> Bindings = nullptr;
	};

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

		virtual void ClearColor(float r, float g, float b, float a = 1.0f) = 0;

		virtual void Clear(ClearMask mask = ClearMask::All) = 0;

		virtual void SetLineWidth(float width) = 0;

		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h) = 0;

		virtual void DrawArrays(ETopologyMode mode, const Ref<VertexArray> &vao, uint32_t count = 0) = 0;

		virtual void DrawElements(ETopologyMode mode, const Ref<VertexArray> &vao, uint32_t count = 0) = 0;

		virtual void DrawElementsBaseVertex(ETopologyMode mode, const Ref<VertexArray> &vao, uint32_t start, uint32_t start_index, uint32_t count = 0, uint32_t instance_count = 0) = 0;

		virtual void DrawElementsRanged(ETopologyMode mode, const Ref<VertexArray>  &vao, uint32_t start, uint32_t end, uint32_t count = 0) = 0;

		virtual void DrawElementsInstanced(ETopologyMode mode, const Ref<VertexArray> &vao, uint32_t instances, uint32_t count = 0) = 0;

		virtual void MultiDrawElementsIndirect(ETopologyMode mode, const BufferBase &indirect, const Ref<VertexArray> &vao, size_t drawCount, size_t stride = 0) = 0;

		virtual void ColorMask(uint8_t r, uint8_t g, uint8_t b, uint8_t a) = 0;

		virtual void SubmitGraph(const RenderGraph &graph, FResourceUpdateList &updateResources) = 0;

		virtual void SubmitResourceUpdate(FResourceUpdateList::UpdateCommand cmd) = 0;

		virtual void DebugPass(const std::string &msg) = 0;

		virtual Ref<ISetManager> CreateSetManager(const Pipeline *pipeline, uint32_t setIndex) = 0;

		virtual Ref<FComputeBindings> CreateComputeBindings(const Ref<Pipeline> &pipeline) = 0;

		virtual AsyncComputeHandle ExecuteComputePass(const Ref<Pipeline> &pipeline, const glm::uvec3 &dispatchSize, const FComputeFunc &builder) = 0;

		virtual EAPI GetAPI() const = 0;

		static Scope<RendererAPI> Create();
	};
} // namespace BHive