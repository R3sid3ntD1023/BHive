#include "batches/CircleRenderBatch.h"
#include "batches/QuadRenderBatch.h"
#include "batches/TextRenderBatch.h"
#include "batches/TextureBatch.h"
#include "core/profiler/CPUGPUProfiler.h"
#include "gfx/font/Font.h"
#include "gfx/font/FontManager.h"
#include "gfx/font/MSDFData.h"
#include "gfx/RenderCommand.h"
#include "gfx/Texture.h"
#include "gfx/VertexArray.h"
#include "QuadRenderer.h"
#include "Renderer.h"
#include "gfx/sprite/Sprite.h"

namespace BHive
{

	struct RenderData2D
	{
		static const uint32_t sMaxQuads = 20'000;
		static const uint32_t sMaxVertices = sMaxQuads * 4;
		static const uint32_t sMaxIndices = sMaxQuads * 6;

		QuadRenderBatch QuadBatch;
		TextRenderBatch TextBatch;
		CircleRenderBatch CircleBatch;
		TextureBatchData TextureBatch;
	};

	void QuadRenderer::Init()
	{
		sRenderData2D = new RenderData2D();
		sRenderData2D->QuadBatch.Init(RenderData2D::sMaxVertices, RenderData2D::sMaxIndices);
		sRenderData2D->CircleBatch.Init(RenderData2D::sMaxVertices, RenderData2D::sMaxIndices);
		sRenderData2D->TextBatch.Init(RenderData2D::sMaxVertices, RenderData2D::sMaxIndices);
		sRenderData2D->TextureBatch.Init();

		sRenderData2D->QuadBatch.SetTextureBatch(&sRenderData2D->TextureBatch);
		sRenderData2D->TextBatch.SetTextureBatch(&sRenderData2D->TextureBatch);
	}

	void QuadRenderer::Shutdown()
	{
		delete sRenderData2D;
	}

	void QuadRenderer::Begin()
	{
		StartBatch();
	}

	void QuadRenderer::End()
	{
		sRenderData2D->QuadBatch.End();
		sRenderData2D->CircleBatch.End();
		sRenderData2D->TextBatch.End();
		sRenderData2D->TextureBatch.End();
	}

	void QuadRenderer::DrawCircle(const FCircleParams &params, const FTransform &transform, int32_t entity_id)
	{
		static glm::vec3 positions[4] = {{-.5f, -.5f, 0.f}, {.5f, -.5f, 0.f}, {.5f, .5f, 0.f}, {-.5f, .5f, 0.f}};

		static uint32_t indices[] = {0, 1, 2, 2, 3, 0};

		if (sRenderData2D->CircleBatch.mIndexCount >= RenderData2D::sMaxIndices)
			sRenderData2D->CircleBatch.NextBatch();

		for (int i = 0; i < 4; i++)
		{
			sRenderData2D->CircleBatch.mVertexCurrentPtr->WorldPosition = transform.ToMat4() * glm::vec4(positions[i] * params.Radius, 1.f);
			sRenderData2D->CircleBatch.mVertexCurrentPtr->LocalPosition = positions[i] * 2.f;
			sRenderData2D->CircleBatch.mVertexCurrentPtr->Color = params.LineColor;
			sRenderData2D->CircleBatch.mVertexCurrentPtr->Thickness = params.Thickness;
			sRenderData2D->CircleBatch.mVertexCurrentPtr->Fade = params.Fade;
			sRenderData2D->CircleBatch.mVertexBufferPtr->EntityID = entity_id;
			sRenderData2D->CircleBatch.mVertexCurrentPtr++;
		}

		auto offset = sRenderData2D->CircleBatch.mVertexCount;
		for (int i = 0; i < 6; i++)
		{
			*sRenderData2D->CircleBatch.mIndexCurrentPtr = offset + indices[i];
			sRenderData2D->CircleBatch.mIndexCurrentPtr++;
		}

		sRenderData2D->CircleBatch.mVertexCount += 4;
		sRenderData2D->CircleBatch.mIndexCount += 6;
	}

	void QuadRenderer::DrawQuad(const FQuadParams &params, const Ref<Texture> &texture, const FTransform &transform, int32_t entity_id)
	{
		static glm::vec3 positions[4] = {{-.5f, -.5f, 0.f}, {.5f, -.5f, 0.f}, {.5f, .5f, 0.f}, {-.5f, .5f, 0.f}};

		static glm::vec2 texcoords[4] = {{0.f, 0.f}, {1.f, 0.f}, {1.f, 1.f}, {0.f, 1.f}};

		FQuadCreateInfo create_info{};
		create_info.Positions = positions;
		create_info.TexCoords = texcoords;
		create_info.Size = params.Size;
		create_info.Color = params.Color;
		create_info.Transform = transform;
		create_info.Tiling = params.Tiling;
		create_info.Flags = params.Flags;
		create_info.TextureRef = texture;
		DrawQuad(create_info, entity_id);
	}

	void QuadRenderer::DrawSprite(const FQuadParams &params, const Ref<Sprite> &sprite, const FTransform &transform, int32_t entity_id)
	{
		if (!sprite)
			return;

		static glm::vec3 positions[4] = {{-.5f, -.5f, 0.f}, {.5f, -.5f, 0.f}, {.5f, .5f, 0.f}, {-.5f, .5f, 0.f}};

		FQuadCreateInfo create_info{};
		create_info.Positions = positions;
		create_info.TexCoords = sprite->GetCoords();
		create_info.Size = params.Size;
		create_info.Color = params.Color;
		create_info.Transform = transform;
		create_info.Tiling = params.Tiling;
		create_info.Flags = params.Flags;
		create_info.TextureRef = sprite->GetSourceTexture();
		DrawQuad(create_info, entity_id);
	}

	void QuadRenderer::DrawBillboard(const FQuadParams &params, const Ref<Texture> &texture, const FTransform &transform, int32_t entity_id)
	{
		const auto &view = Renderer::GetCameraData().View;
		glm::vec3 positions[4] = {{-.5f, -.5f, 0.f}, {.5f, -.5f, 0.f}, {.5f, .5f, 0.f}, {-.5f, .5f, 0.f}};

		const static glm::vec2 texcoords[4] = {{0.f, 0.f}, {1.f, 0.f}, {1.f, 1.f}, {0.f, 1.f}};

		const glm::vec3 camera_right = glm::vec3{view[0][0], view[1][0], view[2][0]};
		const glm::vec3 camera_up = glm::vec3{view[0][1], view[1][1], view[2][1]};
		for (uint32_t i = 0; i < 4; i++)
		{
			auto newposition = glm::vec4(positions[i] * glm::vec3(params.Size, 1), 1.0f);
			glm::vec3 world_space_center = transform.GetTranslation();
			positions[i] = world_space_center + camera_right * newposition.x + camera_up * newposition.y;
		}

		FQuadCreateInfo create_info{};
		create_info.Positions = positions;
		create_info.TexCoords = texcoords;
		create_info.Size = {1, 1};
		create_info.Color = params.Color;
		create_info.Transform = glm::identity<glm::mat4>();
		create_info.Tiling = params.Tiling;
		create_info.Flags = params.Flags;
		create_info.TextureRef = texture;
		DrawQuad(create_info, entity_id);
	}

	void QuadRenderer::DrawQuad(const FQuadCreateInfo &create_info, int32_t entity_id)
	{
		static uint32_t indices[] = {0, 1, 2, 2, 3, 0};

		if (sRenderData2D->QuadBatch.mVertexCount >= RenderData2D::sMaxVertices)
		{
			sRenderData2D->QuadBatch.NextBatch();
		}

		uint32_t texture_index = sRenderData2D->TextureBatch.GetTextureIndex(sRenderData2D->QuadBatch, create_info.TextureRef);

		for (uint32_t i = 0; i < 4; i++)
		{
			sRenderData2D->QuadBatch.mVertexCurrentPtr->Position = create_info.Transform * (glm::vec4(create_info.Positions[i], 1.0f) * glm::vec4(create_info.Size, 1.f, 1.f));
			sRenderData2D->QuadBatch.mVertexCurrentPtr->Normal = glm::transpose(glm::inverse(create_info.Transform)) * glm::vec4(0, 0, 1, 0);
			sRenderData2D->QuadBatch.mVertexCurrentPtr->TexCoord = create_info.TexCoords[i]  * create_info.Tiling;
			sRenderData2D->QuadBatch.mVertexCurrentPtr->Color = create_info.Color;
			sRenderData2D->QuadBatch.mVertexCurrentPtr->TextureIndex = texture_index;
			sRenderData2D->QuadBatch.mVertexCurrentPtr->Flags = create_info.Flags;
			sRenderData2D->QuadBatch.mVertexCurrentPtr->EntityID = entity_id;
			sRenderData2D->QuadBatch.mVertexCurrentPtr++;
		}

		auto offset = sRenderData2D->QuadBatch.mVertexCount;

		for (uint32_t i = 0; i < 6; i++)
		{
			*sRenderData2D->QuadBatch.mIndexCurrentPtr = indices[i] + offset;
			sRenderData2D->QuadBatch.mIndexCurrentPtr++;
		}

		sRenderData2D->QuadBatch.mVertexCount += 4;
		sRenderData2D->QuadBatch.mIndexCount += 6;
	}

	void QuadRenderer::DrawText(float size_arg, const std::string &text, const FTextParams &params, const FTransform &transform, int32_t entity_id)
	{
		auto font = FontManager::Get().GetDefaultFont();
		DrawText(font, size_arg, text, params, transform, entity_id);
	}

	void QuadRenderer::DrawText(const Ref<Font> &font, float size_arg, const std::string &text, const FTextParams &params, const FTransform &transform, int32_t entity_id)
	{
		if (!font)
			return;

		auto texture = font->GetAtlas();
		const auto &fontgeometry = font->GetMSDFData()->FontGeometry;
		const auto &metrics = fontgeometry.getMetrics();

		double scale = (1.0 / (metrics.ascenderY - metrics.descenderY)) * size_arg;
		const float spaceGlyphAdvance = fontgeometry.getGlyph(' ')->getAdvance();

		glm::vec2 texel_size = 1.0f / glm::vec2(texture->GetSize());

		glm::vec2 coords[4];

		glm::dvec2 offset = {};
		for (size_t i = 0; i < text.size(); i++)
		{
			auto c = text[i];

			if (c == '\n')
			{
				offset.x = 0.f;
				offset.y -= (scale * metrics.lineHeight + params.LineSpacing);
				continue;
			}

			if (c == '\t')
			{
				offset.x += 4.0f * (scale * spaceGlyphAdvance + params.Kerning);
				continue;
			}

			auto glyph = fontgeometry.getGlyph(c);
			if (!glyph)
				glyph = fontgeometry.getGlyph('?');
			if (!glyph)
				return;

			GlyphBounds uvs{};
			GlyphBounds quad_bounds{};
			glyph->getQuadAtlasBounds(uvs.Min.x, uvs.Min.y, uvs.Max.x, uvs.Max.y);
			glyph->getQuadPlaneBounds(quad_bounds.Min.x, quad_bounds.Min.y, quad_bounds.Max.x, quad_bounds.Max.y);

			quad_bounds.Min *= scale;
			quad_bounds.Max *= scale;
			quad_bounds.Min += offset;
			quad_bounds.Max += offset;
			uvs.Min *= texel_size;
			uvs.Max *= texel_size;

			glm::vec3 quad[4];
			quad[0] = {quad_bounds.Min, 0};
			quad[1] = {quad_bounds.Max.x, quad_bounds.Min.y, 0};
			quad[2] = {quad_bounds.Max, 0};
			quad[3] = {quad_bounds.Min.x, quad_bounds.Max.y, 0};

			coords[0] = uvs.Min;
			coords[1] = {uvs.Max.x, uvs.Min.y};
			coords[2] = uvs.Max;
			coords[3] = {uvs.Min.x, uvs.Max.y};

			DrawTextQuad(quad, coords, {1, 1}, params.Style, transform, texture, entity_id);

			// bitshift advance to get value in pixels (2^6 = 64)
			if (i < text.size() - 1)
			{
				double advance = glyph->getAdvance();
				char next_character = text[i + 1];
				fontgeometry.getAdvance(advance, c, next_character);

				offset.x += scale * advance + params.Kerning;
			}
		}
	}

	void QuadRenderer::DrawTextQuad(
		const glm::vec3 *points, const glm::vec2 *texcoords, const glm::vec2 &size, const FTextStyle &style, const glm::mat4 &transform, const Ref<Texture> &texture, int32_t entity_id)
	{
		static uint32_t indices[] = {0, 1, 2, 2, 3, 0};

		if (sRenderData2D->TextBatch.mVertexCount >= RenderData2D::sMaxVertices)
		{
			sRenderData2D->TextBatch.NextBatch();
		}

		uint32_t texture_index = sRenderData2D->TextureBatch.GetTextureIndex(sRenderData2D->TextBatch, texture);
	
		for (uint32_t i = 0; i < 4; i++)
		{
			sRenderData2D->TextBatch.mVertexCurrentPtr->Position = transform * (glm::vec4(points[i], 1.0f) * glm::vec4(size, 1.f, 1.f));
			sRenderData2D->TextBatch.mVertexCurrentPtr->TexCoord = texcoords[i];
			sRenderData2D->TextBatch.mVertexCurrentPtr->Color = style.TextColor;
			sRenderData2D->TextBatch.mVertexCurrentPtr->Thickness = {style.Thickness, style.Smoothness};
			sRenderData2D->TextBatch.mVertexCurrentPtr->Outline = {style.OutlineThickness, style.OutlineSmoothness};
			sRenderData2D->TextBatch.mVertexCurrentPtr->OutlineColor = style.OutlineColor;
			sRenderData2D->TextBatch.mVertexCurrentPtr->Texture = texture_index;
			sRenderData2D->TextBatch.mVertexCurrentPtr->EntityID = entity_id;
			sRenderData2D->TextBatch.mVertexCurrentPtr++;
		}

		auto offset = sRenderData2D->TextBatch.mVertexCount;

		for (uint32_t i = 0; i < 6; i++)
		{
			*sRenderData2D->TextBatch.mIndexCurrentPtr = indices[i] + offset;
			sRenderData2D->TextBatch.mIndexCurrentPtr++;
		}

		sRenderData2D->TextBatch.mVertexCount += 4;
		sRenderData2D->TextBatch.mIndexCount += 6;
	}

	void QuadRenderer::StartBatch()
	{
		sRenderData2D->TextBatch.StartBatch();
		sRenderData2D->QuadBatch.StartBatch();
		sRenderData2D->CircleBatch.StartBatch();
	}

	void QuadRenderer::Flush()
	{
		GPU_PROFILER_FUNCTION();

		sRenderData2D->TextBatch.Flush();
		sRenderData2D->QuadBatch.Flush();
		sRenderData2D->CircleBatch.Flush();
	}
} // namespace BHive