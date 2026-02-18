#pragma once

#include "core/Core.h"
#include "core/events/Event.h"
#include "Enumerations.h"
#include "RenderGraph.h"

namespace BHive
{
	class VertexArray;
	class BufferBase;

	

	struct MultiDrawIndirectCommand
	{
		uint32_t Count;
		uint32_t InstanceCount;
		uint32_t FirstIndex;
		int32_t BaseVertex;
		uint32_t BaseInstance;
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

		virtual void DrawElementsBaseVertex(ETopologyMode mode, const VertexArray &vao, uint32_t start, uint32_t start_index, uint32_t count = 0, uint32_t instance_count = 0) = 0;

		virtual void DrawElementsRanged(ETopologyMode mode, const VertexArray &vao, uint32_t start, uint32_t end, uint32_t count = 0) = 0;

		virtual void DrawElementsInstanced(ETopologyMode mode, const VertexArray &vao, uint32_t instances, uint32_t count = 0) = 0;

		virtual void MultiDrawElementsIndirect(ETopologyMode mode, const BufferBase &indirect, const VertexArray &vao, size_t drawCount, size_t stride = 0) = 0;

		virtual void EnableDepth() = 0;

		virtual void DisableDepth() = 0;

		virtual void DepthFunc(uint32_t func) = 0;

		virtual void CullFront() = 0;

		virtual void CullBack() = 0;

		virtual void SetCullEnabled(bool enabled) = 0;

		virtual void ColorMask(uint8_t r, uint8_t g, uint8_t b, uint8_t a) = 0;

		virtual void EnableDepthMask(bool mask) = 0;

		virtual void EnableBlend(bool enabled) = 0;

		virtual void AttachTextureToFramebuffer(uint32_t attachment, uint32_t texture, uint32_t framebuffer) = 0;

		virtual void ExecuteGraph(const RenderGraph &graph, Window *defaultWindow) = 0;

		virtual EAPI GetAPI() const = 0;

		static Scope<RendererAPI> Create();
	};
} // namespace BHive