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
		static BHIVE void Init();
		static BHIVE void Shutdown();

		static BHIVE void Begin(const glm::mat4 &view);
		static BHIVE void End();

		static BHIVE void
		DrawQuad(const glm::vec2 &size, const Color &color, const FTransform &transform, const Ref<Texture> &texture, QuadRendererFlags flags = 0);

		static BHIVE void
		DrawSprite(const glm::vec2 &size, const Color &color, const FTransform &transform, const Ref<Sprite> &sprite, QuadRendererFlags flags = 0);

		static BHIVE void DrawBillboard(
			const glm::vec2 &size, const Color &color, const FTransform &transform, const Ref<Texture> &texture, QuadRendererFlags flags = 0);

		static BHIVE void DrawQuad(
			const glm::vec3 *points, const glm::vec2 *texcoords, const glm::vec2 &size, const Color &color, const glm::mat4 &transform,
			const Ref<Texture> &texture, QuadRendererFlags flags = 0);

	private:
		static BHIVE void StartBatch();
		static BHIVE void NextBatch();
		static BHIVE void Flush();

	protected:
		struct RenderData;
		static RenderData *sData;
	};
} // namespace BHive