#pragma once

#include "gfx/Color.h"
#include "core/math/Transform.h"
#include "batches/CircleRenderBatch.h"
#include "batches/QuadRenderBatch.h"
#include "batches/TextRenderBatch.h"
#include "batches/TextureBatch.h"
#include "ViewSystem.h"
#include "gfx/registries/Handles.h"

namespace BHive
{
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
		TexturePtr Texture;
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
		TexturePtr Texture;
		FTransform Transform;
		int32_t EntityID = -1;
	};

	class Renderer;

	class BHIVE_API QuadRenderer
	{
	public:
		void Initialize();

		void BeginRecording();

		void Flush(Renderer &renderer);

		void DrawCircle(const FCircleParams &params, const FTransform &transform, int32_t entity = -1);

		void DrawQuad(const FQuadParams &params, TexturePtr texture, const FTransform &transform, int32_t entity = -1);

		void DrawSprite(const FQuadParams &params, SpritePtr sprite, const FTransform &transform, int32_t entity = -1);

		void DrawBillboard(const FView &view, const FQuadParams &params, TexturePtr texture, const FTransform &transform, int32_t entity = -1);

		void DrawQuad(const FQuadCreateInfo &create_info, int32_t entity = -1);

		void DrawText(FontPtr font, float size, const std::string &text, const FTextParams &params = {}, const FTransform &transform = {}, int32_t entity = -1);

	private:
		void DrawTextQuad(const glm::vec3 *points, const glm::vec2 *texcoords, const glm::vec2 &size, const FTextStyle &style, const glm::mat4 &transform, TexturePtr texture, int32_t entity = -1);

	private:
		QuadRenderBatch QuadBatch;
		TextRenderBatch TextBatch;
		CircleRenderBatch CircleBatch;
		TextureBatchData TextureBatch;
	};
} // namespace BHive