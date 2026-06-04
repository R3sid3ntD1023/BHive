
#include "core/profiler/CPUGPUProfiler.h"
#include "gfx/font/Font.h"
#include "gfx/font/FontManager.h"
#include "gfx/font/MSDFData.h"
#include "gfx/Texture.h"
#include "QuadRenderer.h"
#include "Renderer.h"
#include "gfx/sprite/Sprite.h"

namespace BHive
{

	void QuadRenderer::Initialize()
	{
		QuadBatch.Initialize();
		CircleBatch.Initialize();
		TextBatch.Initialize();
		TextureBatch.Initialize();

		QuadBatch.SetTextureBatch(&TextureBatch);
		TextBatch.SetTextureBatch(&TextureBatch);
	}

	void QuadRenderer::Begin()
	{
		CircleBatch.StartBatch();
		QuadBatch.StartBatch();
		TextBatch.StartBatch();
	}

	void QuadRenderer::End(Renderer &renderer)
	{
		GPU_PROFILER_FUNCTION();

		CircleBatch.Flush(renderer);
		QuadBatch.Flush(renderer);
		TextBatch.Flush(renderer);
	}

	void QuadRenderer::DrawCircle(const FCircleParams &params, const FTransform &transform, int32_t entity_id)
	{
		static glm::vec3 positions[4] = {{-.5f, -.5f, 0.f}, {.5f, -.5f, 0.f}, {.5f, .5f, 0.f}, {-.5f, .5f, 0.f}};

		static uint32_t indices[] = {0, 1, 2, 2, 3, 0};

		if (CircleBatch.NeedsFlush(4, 6))
			CircleBatch.NextBatch(Renderer::Get());

		auto offset = CircleBatch.GetBuffer().GetVertexCount();

		for (int i = 0; i < 4; i++)
		{
			auto v = CircleBatch.GetBuffer().PushVertex();
			v->WorldPosition = transform.ToMat4() * glm::vec4(positions[i] * params.Radius, 1.f);
			v->LocalPosition = positions[i] * 2.f;
			v->Color = params.LineColor;
			v->Thickness = params.Thickness;
			v->Fade = params.Fade;
			v->EntityID = entity_id;
		}

		for (int i = 0; i < 6; i++)
		{
			*CircleBatch.GetBuffer().PushIndex() = offset + indices[i];
		}
	}

	void QuadRenderer::DrawQuad(const FQuadParams &params, const Ref<Texture2D> &texture, const FTransform &transform, int32_t entity_id)
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

	void QuadRenderer::DrawBillboard(const FQuadParams &params, const Ref<Texture2D> &texture, const FTransform &transform, int32_t entity_id)
	{
		const auto &view = Renderer::Get().GetCameraData().View;
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

		if (QuadBatch.NeedsFlush(4, 6))
		{
			QuadBatch.NextBatch(Renderer::Get());
		}

		uint32_t texture_index = TextureBatch.GetTextureIndex(create_info.TextureRef);
		if (texture_index == -1)
		{
			QuadBatch.NextBatch(Renderer::Get());
			texture_index = TextureBatch.GetTextureIndex(create_info.TextureRef);
		}

		for (uint32_t i = 0; i < 4; i++)
		{
			auto v = QuadBatch.GetBuffer().PushVertex();
			v->Position = create_info.Transform * (glm::vec4(create_info.Positions[i], 1.0f) * glm::vec4(create_info.Size, 1.f, 1.f));
			v->Normal = glm::transpose(glm::inverse(create_info.Transform)) * glm::vec4(0, 0, 1, 0);
			v->TexCoord = create_info.TexCoords[i]  * create_info.Tiling;
			v->Color = create_info.Color;
			v->TextureIndex = texture_index;
			v->Flags = create_info.Flags;
			v->EntityID = entity_id;
		}

		auto offset = QuadBatch.GetBuffer().GetVertexCount();

		for (uint32_t i = 0; i < 6; i++)
		{
			*QuadBatch.GetBuffer().PushIndex() = indices[i] + offset;
		}
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
		const glm::vec3 *points, const glm::vec2 *texcoords, const glm::vec2 &size, const FTextStyle &style, const glm::mat4 &transform, const Ref<Texture2D> &texture, int32_t entity_id)
	{
		static uint32_t indices[] = {0, 1, 2, 2, 3, 0};


		if (TextBatch.NeedsFlush(4, 6))
		{
			TextBatch.NextBatch(Renderer::Get());
		}

		uint32_t texture_index = TextureBatch.GetTextureIndex(texture);
		if (texture_index == -1)
		{
			QuadBatch.NextBatch(Renderer::Get());
		}
	
		auto offset = TextBatch.GetBuffer().GetVertexCount();

		for (uint32_t i = 0; i < 4; i++)
		{
			auto v = TextBatch.GetBuffer().PushVertex();
			v->Position = transform * (glm::vec4(points[i], 1.0f) * glm::vec4(size, 1.f, 1.f));
			v->TexCoord = texcoords[i];
			v->Color = style.TextColor;
			v->Thickness = {style.Thickness, style.Smoothness};
			v->Outline = {style.OutlineThickness, style.OutlineSmoothness};
			v->OutlineColor = style.OutlineColor;
			v->Texture = texture_index;
			v->EntityID = entity_id;
		}

		for (uint32_t i = 0; i < 6; i++)
		{
			*TextBatch.GetBuffer().PushIndex() = indices[i] + offset;
		}
	}
} // namespace BHive