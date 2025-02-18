#pragma once

#include "gfx/Color.h"
#include "math/Transform.h"

namespace BHive
{
	class Texture;

	class Font;

	struct FontStyle
	{
		Color TextColor = 0xFFFFFFFF;
		float Thickness = .5f;
		float Smoothness = 0.f;

		Color OutlineColor = 0xFF000000;
		float OutlineThickness = 0.f;
		float OutlineSmoothness = 0.1f;
	};

	struct FontParams
	{
		float Kerning = 0.0f;
		float LineSpacing = 0.f;
		FontStyle Style{};
	};

	class TextRenderer
	{
	public:
		static void Init();
		static void Shutdown();

		static void Begin();
		static void End();

		static void
		DrawText(const Ref<Font> &font, float size, const std::string &text, const FontParams &style = {}, const FTransform &transform = {});

	private:
		static void DrawQuad(
			const glm::vec3 *points, const glm::vec2 *texcoords, const glm::vec2 &size, const FontStyle &style, const glm::mat4 &transform,
			const Ref<Texture> &texture);

	private:
		static void StartBatch();
		static void NextBatch();
		static void Flush();

	protected:
		struct RenderData;
		static RenderData *sData;
	};
} // namespace BHive