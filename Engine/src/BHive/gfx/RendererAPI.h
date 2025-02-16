#pragma once

#include "core/Core.h"
#include "gfx/VertexArray.h"

namespace BHive
{
	enum EDrawMode
	{
		Lines = 0x0001,
		Triangles = 0x0004
	};

	enum ClearBitMask : int
	{
		Buffer_Depth = 0x00000100,
		Buffer_Stencil = 0x00000400,
		Buffer_Color = 0x00004000,
	};

	class RendererAPI
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
		virtual void ClearColor(float r, float g, float b, float a = 1.0f) = 0;
		virtual void Clear(int mask = Buffer_Color | Buffer_Depth | Buffer_Stencil) = 0;

		virtual void SetLineWidth(float width) = 0;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h) = 0;

		virtual void DrawArrays(EDrawMode mode, const VertexArray &vao, uint32_t count = 0) = 0;
		virtual void DrawElements(EDrawMode mode, const VertexArray &vao, uint32_t count = 0) = 0;
		virtual void DrawElementsBaseVertex(EDrawMode mode, const VertexArray &vao, uint32_t start, uint32_t start_index, uint32_t count = 0) = 0;
		virtual void DrawElementsRanged(EDrawMode mode, const VertexArray &vao, uint32_t start, uint32_t end, uint32_t count = 0) = 0;
		virtual void DrawElementsInstanced(EDrawMode mode, const VertexArray &vao, uint32_t instances, uint32_t count = 0) = 0;

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

		virtual unsigned CheckError(const char *file, int line) = 0;

		static Scope<RendererAPI> Create();
		static EAPI GetAPI() { return sAPI; };

	private:
		static inline EAPI sAPI = Opengl;
	};
}