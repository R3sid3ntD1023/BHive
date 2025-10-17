#pragma once

#include "RendererAPI.h"

namespace BHive
{

	class BHIVE_API RenderCommand
	{
	public:
		static void Init();

		static void Shutdown();

		static void ClearColor(float r, float g, float b, float a = 1.0f);

		static void Clear(int mask = Buffer_Color | Buffer_Depth | Buffer_Stencil);

		static void SetLineWidth(float width);

		static void SetViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h);

		static void DrawArrays(EDrawMode mode, const VertexArray &vao, uint32_t count = 0);

		static void DrawElements(EDrawMode mode, const VertexArray &vao, uint32_t count = 0);

		static void DrawElementsBaseVertex(EDrawMode mode, const VertexArray &vao, uint32_t start, uint32_t start_index, uint32_t count = 0, uint32_t instance_count = 0);

		static void DrawElementsRanged(EDrawMode mode, const VertexArray &vao, uint32_t start, uint32_t end, uint32_t count = 0);
		static void DrawElementsInstanced(EDrawMode mode, const VertexArray &vao, uint32_t instances, uint32_t count = 0);

		static void MultiDrawElementsIndirect(EDrawMode mode, const BufferBase &indirect, const VertexArray &vao, const void *data, size_t drawCount, size_t stride = 0);

		static void EnableDepth();
		static void DisableDepth();
		static void DepthFunc(uint32_t func);

		static void CullFront();
		static void CullBack();

		static void SetCullEnabled(bool enabled);

		static void ColorMask(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
		static void EnableDepthMask(bool mask);
		static void EnableBlend(bool enabled);

		static void AttachTextureToFramebuffer(uint32_t attachment, uint32_t texture, uint32_t framebuffer);

		static void *CreateShader(const uint32_t *data, size_t size);

		static void BeginFrame();

		static void EndFrame();


		static RendererAPI *GetAPI() { return sRendererAPI.get(); }

	private:
		static inline Scope<RendererAPI> sRendererAPI = CreateScope<RendererAPI>();
	};
} // namespace BHive
