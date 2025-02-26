#include "RenderCommand.h"
#include <glad/glad.h>

namespace BHive
{
	void RenderCommand::Init()
	{
		sRendererAPI->Init();
	}

	void RenderCommand::ClearColor(float r, float g, float b, float a)
	{
		sRendererAPI->ClearColor(r, g, b, a);
	}

	void RenderCommand::Clear(int mask)
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

	void RenderCommand::DrawArrays(EDrawMode mode, const VertexArray &vao, uint32_t count)
	{
		sRendererAPI->DrawArrays(mode, vao, count);
	}

	void RenderCommand::DrawElements(EDrawMode mode, const VertexArray &vao, uint32_t count)
	{
		sRendererAPI->DrawElements(mode, vao, count);
	}

	void RenderCommand::DrawElementsBaseVertex(
		EDrawMode mode, const VertexArray &vao, uint32_t start, uint32_t start_index, uint32_t count)
	{
		sRendererAPI->DrawElementsBaseVertex(mode, vao, start, start_index, count);
	}

	void
	RenderCommand::DrawElementsRanged(EDrawMode mode, const VertexArray &vao, uint32_t start, uint32_t end, uint32_t count)
	{
		sRendererAPI->DrawElementsRanged(mode, vao, start, end, count);
	}

	void RenderCommand::DrawElementsInstanced(EDrawMode mode, const VertexArray &vao, uint32_t instances, uint32_t count)
	{
		sRendererAPI->DrawElementsInstanced(mode, vao, instances, count);
	}

	void RenderCommand::MultiDrawElementsIndirect(
		uint32_t buffer, EDrawMode mode, const VertexArray &vao, size_t numMeshes, size_t stride)
	{
		sRendererAPI->MultiDrawElementsIndirect(buffer, mode, vao, numMeshes, stride);
	}

	void RenderCommand::EnableDepth()
	{
		sRendererAPI->EnableDepth();
	}

	void RenderCommand::DisableDepth()
	{
		sRendererAPI->DisableDepth();
	}

	void RenderCommand::DepthFunc(uint32_t func)
	{
		sRendererAPI->DepthFunc(func);
	}

	void RenderCommand::CullFront()
	{
		sRendererAPI->CullFront();
	}

	void RenderCommand::CullBack()
	{
		sRendererAPI->CullBack();
	}

	void RenderCommand::SetCullEnabled(bool enabled)
	{
		sRendererAPI->SetCullEnabled(enabled);
	}

	void RenderCommand::ColorMask(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
	{
		sRendererAPI->ColorMask(r, g, b, a);
	}

	void RenderCommand::EnableDepthMask(bool mask)
	{
		sRendererAPI->EnableDepthMask(mask);
	}

	void RenderCommand::EnableBlend(bool enabled)
	{
		sRendererAPI->EnableBlend(enabled);
	}

	void RenderCommand::AttachTextureToFramebuffer(uint32_t attachment, uint32_t texture, uint32_t framebuffer)
	{
		sRendererAPI->AttachTextureToFramebuffer(attachment, texture, framebuffer);
	}

	unsigned RenderCommand::CheckError(const char *file, int line)
	{
		return sRendererAPI->CheckError(file, line);
	}

	Scope<RendererAPI> RenderCommand::sRendererAPI = RendererAPI::Create();
} // namespace BHive