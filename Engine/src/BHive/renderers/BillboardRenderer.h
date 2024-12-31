#pragma once

#include "core/Core.h"
#include "math/Transform.h"
#include "gfx/Color.h"

namespace BHive
{
	class Texture;

	class BillboardRenderer
	{
	public:
		static void Init();
		static void Shutdown();

		static void Begin();
		static void End();

		static void DrawBillboard(const glm::vec2& size, const Color& color, const FTransform& transform,
			const Ref<Texture>& texture);

	private:

		static void StartBatch();
		static void NextBatch();
		static void Flush();

	protected:
		struct RenderData;
		static RenderData* sData;
	};
}