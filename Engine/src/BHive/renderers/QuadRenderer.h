#pragma once

#include "gfx/Color.h"
#include "math/Transform.h"

namespace BHive
{
	class Texture;
	class Sprite;
	class Font;

	struct FTextStyle
	{
		Color TextColor = 0xFFFFFFFF;
		float Thickness = .5f;
		float Smoothness = 0.f;

		Color OutlineColor = 0xFF000000;
		float OutlineThickness = 0.f;
		float OutlineSmoothness = 0.1f;
	};

	struct FTextParams
	{
		float Kerning = 0.0f;
		float LineSpacing = 0.f;
		FTextStyle Style{};
	};

	struct FCircleParams
	{
		Color LineColor = 0xffffffff;
		float Thickness = 1.0f;
		float Fade = 0.005f;
		float Radius = 1.f;
	};

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

		static void DrawCircle(const FCircleParams &params, const FTransform &transform);

		static void DrawQuad(
			const glm::vec2 &size, const Color &color, const FTransform &transform, const Ref<Texture> &texture,
			QuadRendererFlags flags = 0);

		static void DrawSprite(
			const glm::vec2 &size, const Color &color, const FTransform &transform, const Ref<Sprite> &sprite,
			QuadRendererFlags flags = 0);

		static void DrawBillboard(
			const glm::vec2 &size, const Color &color, const FTransform &transform, const Ref<Texture> &texture,
			QuadRendererFlags flags = 0);

		static void DrawQuad(
			const glm::vec3 *points, const glm::vec2 *texcoords, const glm::vec2 &size, const Color &color,
			const glm::mat4 &transform, const Ref<Texture> &texture, QuadRendererFlags flags = 0);

		static void
		DrawText(float size, const std::string &text, const FTextParams &params = {}, const FTransform &transform = {});

		static void DrawText(
			const Ref<Font> &font, float size, const std::string &text, const FTextParams &params = {},
			const FTransform &transform = {});

	private:
		static void DrawTextQuad(
			const glm::vec3 *points, const glm::vec2 *texcoords, const glm::vec2 &size, const FTextStyle &style,
			const glm::mat4 &transform, const Ref<Texture> &texture);

		static uint32_t
		GetTextureIndex(struct IRenderDataBase *data, struct TextureData &textures, const Ref<Texture> &texture);

	private:
		static void StartBatch();
		static void Flush();

	protected:
		struct RenderData;
		struct TextData;
		struct CircleData;

		static inline RenderData *sData = nullptr;
		static inline TextData *sTextData = nullptr;
		static inline CircleData *sCircleData = nullptr;
	};
} // namespace BHive