#include "RenderCommand.h"

namespace BHive
{
	void RenderCommand::Init()
	{
		sRendererAPI->Init();
	}

	void RenderCommand::Shutdown()
	{
		sRendererAPI->Shutdown();
	}

	void RenderCommand::WaitIdle()
	{
		sRendererAPI->WaitIdle();
	}

	void RenderCommand::ClearColor(float r, float g, float b, float a)
	{
		sRendererAPI->ClearColor(r, g, b, a);
	}

	void RenderCommand::Clear(ClearMask mask)
	{
		sRendererAPI->Clear(mask);
	}

	void RenderCommand::SetLineWidth(float width)
	{
		sRendererAPI->SetLineWidth(width);
	}

	void RenderCommand::SetViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h)
	{
		sRendererAPI->SetViewport(x, y, w, h);
	}

	void RenderCommand::DrawArrays(ETopologyMode mode, const Ref<VertexArray> &vao, uint32_t count)
	{
		sRendererAPI->DrawArrays(mode, vao, count);
	}

	void RenderCommand::DrawElements(ETopologyMode mode, const Ref<VertexArray> &vao, uint32_t count)
	{
		sRendererAPI->DrawElements(mode, vao, count);
	}

	void RenderCommand::DrawElementsBaseVertex(ETopologyMode mode, const VertexArray &vao, uint32_t start, uint32_t start_index, uint32_t count, uint32_t instance_count)
	{
		sRendererAPI->DrawElementsBaseVertex(mode, vao, start, start_index, count, instance_count);
	}

	void RenderCommand::DrawElementsRanged(ETopologyMode mode, const VertexArray &vao, uint32_t start, uint32_t end, uint32_t count)
	{
		sRendererAPI->DrawElementsRanged(mode, vao, start, end, count);
	}

	void RenderCommand::DrawElementsInstanced(ETopologyMode mode, const VertexArray &vao, uint32_t instances, uint32_t count)
	{
		sRendererAPI->DrawElementsInstanced(mode, vao, instances, count);
	}

	void RenderCommand::MultiDrawElementsIndirect(ETopologyMode mode, const BufferBase &indirect, const VertexArray &vao, size_t drawCount, size_t stride)
	{
		sRendererAPI->MultiDrawElementsIndirect(mode, indirect, vao,  drawCount, stride);
	}

	void RenderCommand::Dispath(uint32_t x, uint32_t y, uint32_t z)
	{
		sRendererAPI->Dispath(x, y, z);
	}

	
	void RenderCommand::ColorMask(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
	{
		sRendererAPI->ColorMask(r, g, b, a);
	}

	Scope<RendererAPI> RenderCommand::sRendererAPI = RendererAPI::Create();

} // namespace BHive