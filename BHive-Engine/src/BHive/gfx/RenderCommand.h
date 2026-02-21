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

		static void Clear(ClearMask mask = ClearMask::All);

		static void SetLineWidth(float width);

		static void SetViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h);

		static void DrawArrays(ETopologyMode mode, const Ref<VertexArray> &vao, uint32_t count = 0);

		static void DrawElements(ETopologyMode mode, const Ref<VertexArray> &vao, uint32_t count = 0);

		static void DrawElementsBaseVertex(ETopologyMode mode, const VertexArray &vao, uint32_t start, uint32_t start_index, uint32_t count = 0, uint32_t instance_count = 0);

		static void DrawElementsRanged(ETopologyMode mode, const VertexArray &vao, uint32_t start, uint32_t end, uint32_t count = 0);

		static void DrawElementsInstanced(ETopologyMode mode, const VertexArray &vao, uint32_t instances, uint32_t count = 0);

		static void MultiDrawElementsIndirect(ETopologyMode mode, const BufferBase &indirect, const VertexArray &vao,  size_t drawCount, size_t stride = 0);

		static void Dispath(uint32_t x, uint32_t y, uint32_t z);

		static void ColorMask(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

		static RendererAPI::EAPI GetGraphicsAPI() { return sRendererAPI->GetAPI(); }

		template <typename TAPI>
			requires(std::is_base_of_v<RendererAPI, TAPI>)
		static TAPI *GetRendererAPI()
		{
			return Cast<TAPI>(sRendererAPI.get());
		}

	private:
		static Scope<RendererAPI> sRendererAPI;
	};
} // namespace BHive
