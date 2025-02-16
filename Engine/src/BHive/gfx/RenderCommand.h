#pragma once

#include "RendererAPI.h"

namespace BHive
{
	class RenderCommand
	{
	public:
		static BHIVE void Init();

		static BHIVE void ClearColor(float r, float g, float b, float a = 1.0f);
		static BHIVE void Clear(int mask = Buffer_Color | Buffer_Depth | Buffer_Stencil);

		static BHIVE void SetLineWidth(float width);
		static void SetViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h);

		static BHIVE void DrawArrays(EDrawMode mode, const VertexArray &vao, uint32_t count = 0);
		static BHIVE void DrawElements(EDrawMode mode, const VertexArray &vao, uint32_t count = 0);
		static void DrawElementsBaseVertex(EDrawMode mode, const VertexArray &vao, uint32_t start, uint32_t start_index, uint32_t count = 0);
		static void DrawElementsRanged(EDrawMode mode, const VertexArray &vao, uint32_t start, uint32_t end, uint32_t count = 0);
		static BHIVE void DrawElementsInstanced(EDrawMode mode, const VertexArray &vao, uint32_t instances, uint32_t count = 0);

		static BHIVE void EnableDepth();
		static BHIVE void DisableDepth();
		static void DepthFunc(uint32_t func);

		static void CullFront();
		static void CullBack();

		static void SetCullEnabled(bool enabled);

		static void ColorMask(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
		static void EnableDepthMask(bool mask);
		static void EnableBlend(bool enabled);

		static void AttachTextureToFramebuffer(uint32_t attachment, uint32_t texture, uint32_t framebuffer);

		static unsigned CheckError(const char *file, int line);

	private:
		static Scope<RendererAPI> sRendererAPI;
	};
} // namespace BHive

#define GL_CHECK_ERROR() ::BHive::RenderCommand::CheckError(__FILE__, __LINE__)