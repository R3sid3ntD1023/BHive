#pragma once

#include "gfx/Color.h"
#include "core/math/Transform.h"

namespace BHive
{
	class Texture;
	class Sprite;
	class Font;

	struct FTextStyle
	{
		FColor TextColor = 0xFFFFFFFF;
		float Thickness = .5f;
		float Smoothness = 0.f;

		FColor OutlineColor = 0xFF000000;
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
		float Radius = 1.f;
		FColor LineColor = 0xffffffff;
		float Thickness = 1.0f;
		float Fade = 0.005f;
	};

	enum QuadRendererFlags_
	{
		QuadRendererFlags_None,
		QuadRendererFlags_Lit = BIT(0)
	};

	typedef unsigned QuadRendererFlags;

	struct FQuadParams
	{
		glm::vec2 Size{1, 1};
		glm::vec2 Tiling{1, 1};
		FColor Color{0xffffffff};
		QuadRendererFlags Flags{0};
	};

	struct FQuadCreateInfo
	{
		const glm::vec3 *Positions;
		const glm::vec2 *TexCoords;

		FColor Color = FColor::White;
		Ref<Texture> TextureRef{nullptr};
		glm::vec2 Size{1, 1};
		glm::vec2 Tiling{1, 1};
		glm::mat4 Transform{1.0f};
		QuadRendererFlags Flags = 0;
	};

	struct FBillboardCreateInfo
	{
		glm::vec2 Size{1, 1};
		glm::vec2 Tiling{1, 1};
		FColor Color{0xffffffff};
		QuadRendererFlags Flags{0};
		Ref<Texture> TextureRef{nullptr};
		FTransform Transform;
		int32_t EntityID = -1;
	};

	class BHIVE_API QuadRenderer
	{
	public:
		static void Init();
		static void Shutdown();

		static void Begin();
		static void End();

		static void DrawCircle(const FCircleParams &params, const FTransform &transform, int32_t entity = -1);

		static void DrawQuad(const FQuadParams &params, const Ref<Texture> &texture, const FTransform &transform, int32_t entity = -1);

		static void DrawSprite(const FQuadParams &params, const Ref<Sprite> &sprite, const FTransform &transform, int32_t entity = -1);

		static void DrawBillboard(const FQuadParams &params, const Ref<Texture> &texture, const FTransform &transform, int32_t entity = -1);

		static void DrawQuad(const FQuadCreateInfo &create_info, int32_t entity = -1);

		static void DrawText(float size, const std::string &text, const FTextParams &params = {}, const FTransform &transform = {}, int32_t entity = -1);

		static void DrawText(const Ref<Font> &font, float size, const std::string &text, const FTextParams &params = {}, const FTransform &transform = {}, int32_t entity = -1);

		static void Flush();

	private:
		static void
		DrawTextQuad(const glm::vec3 *points, const glm::vec2 *texcoords, const glm::vec2 &size, const FTextStyle &style, const glm::mat4 &transform, const Ref<Texture> &texture, int32_t entity = -1);

	private:
		static void StartBatch();

		static inline struct RenderData2D *sRenderData2D = nullptr;
	};
} // namespace BHive