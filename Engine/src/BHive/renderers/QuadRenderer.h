#pragma once

#include "gfx/Color.h"
#include "math/Transform.h"

namespace BHive
{
	class Texture;
	class Sprite;

	enum QuadRendererFlags_
	{
		QuadRendererFlags_None,
		QuadRendererFlags_Lit = BIT(0)
	};

	typedef unsigned QuadRendererFlags;

	class QuadRenderer
	{
	public:
		static void Init();
		static void Shutdown();

		static void Begin(const glm::mat4 &view);
		static void End();

		static void
		DrawQuad(const glm::vec2 &size, const Color &color, const FTransform &transform, const Ref<Texture> &texture, QuadRendererFlags flags = 0);

		static void
		DrawSprite(const glm::vec2 &size, const Color &color, const FTransform &transform, const Ref<Sprite> &sprite, QuadRendererFlags flags = 0);

		static void DrawBillboard(
			const glm::vec2 &size, const Color &color, const FTransform &transform, const Ref<Texture> &texture, QuadRendererFlags flags = 0);

		static void DrawQuad(
			const glm::vec3 *points, const glm::vec2 *texcoords, const glm::vec2 &size, const Color &color, const glm::mat4 &transform,
			const Ref<Texture> &texture, QuadRendererFlags flags = 0);

	private:
		static void StartBatch();
		static void NextBatch();
		static void Flush();

	protected:
		struct RenderData;
		static RenderData *sData;
	};
} // namespace BHive