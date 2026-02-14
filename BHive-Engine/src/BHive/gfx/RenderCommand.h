#pragma once

#include "RendererAPI.h"

namespace BHive
{

	class BHIVE_API RenderCommand
	{
	public:
		static void Init();

		static void Shutdown();

		static void WaitIdle();

		static void ClearColor(float r, float g, float b, float a = 1.0f);

		static void Clear(int mask = Buffer_Color | Buffer_Depth | Buffer_Stencil);

		static void SetLineWidth(float width);

		static void SetViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h);

		static void DrawArrays(ETopologyMode mode, const Ref<VertexArray> &vao, uint32_t count = 0);

		static void DrawElements(ETopologyMode mode, const Ref<VertexArray> &vao, uint32_t count = 0);

		static void DrawElementsBaseVertex(ETopologyMode mode, const VertexArray &vao, uint32_t start, uint32_t start_index, uint32_t count = 0, uint32_t instance_count = 0);

		static void DrawElementsRanged(ETopologyMode mode, const VertexArray &vao, uint32_t start, uint32_t end, uint32_t count = 0);

		static void DrawElementsInstanced(ETopologyMode mode, const VertexArray &vao, uint32_t instances, uint32_t count = 0);

		static void MultiDrawElementsIndirect(ETopologyMode mode, const BufferBase &indirect, const VertexArray &vao, const void *data, size_t drawCount, size_t stride = 0);

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

		static RendererAPI::EAPI GetRendererAPI() { return sRendererAPI->GetAPI(); }

		template <typename TAPI>
			requires(std::is_base_of_v<RendererAPI, TAPI>)
		static TAPI *GetAPI()
		{
			return dynamic_cast<TAPI *>(sRendererAPI.get());
		}

	private:
		static Scope<RendererAPI> sRendererAPI;
	};
} // namespace BHive
