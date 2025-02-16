#pragma once

#include "gfx/RendererAPI.h"

namespace BHive
{
	class GLRendererAPI : public RendererAPI
	{
	public:
		GLRendererAPI() = default;

		virtual void Init();
		virtual void ClearColor(float r, float g, float b, float a = 1.0f);
		virtual void Clear(int mask = Buffer_Color | Buffer_Depth | Buffer_Stencil);

		virtual void SetLineWidth(float width);
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h);

		virtual void DrawArrays(EDrawMode mode, const VertexArray &vao, uint32_t count = 0);
		virtual void DrawElements(EDrawMode mode, const VertexArray &vao, uint32_t count = 0);
		virtual void DrawElementsBaseVertex(EDrawMode mode, const VertexArray &vao, uint32_t start, uint32_t start_index, uint32_t count = 0);
		virtual void DrawElementsRanged(EDrawMode mode, const VertexArray &vao, uint32_t start, uint32_t end, uint32_t count = 0);
		virtual void DrawElementsInstanced(EDrawMode mode, const VertexArray &vao, uint32_t instances, uint32_t count = 0);

		virtual void EnableDepth();
		virtual void DisableDepth();
		virtual void DepthFunc(uint32_t func);

		virtual void CullFront();
		virtual void CullBack();

		virtual void SetCullEnabled(bool enabled);

		virtual void ColorMask(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
		virtual void EnableDepthMask(bool mask);
		virtual void EnableBlend(bool enabled);

		virtual void AttachTextureToFramebuffer(uint32_t attachment, uint32_t texture, uint32_t framebuffer);

		virtual unsigned CheckError(const char *file, int line);
	};
}