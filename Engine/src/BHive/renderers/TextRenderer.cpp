#include "TextRenderer.h"
#include "gfx/Shader.h"
#include "gfx/Texture.h"
#include "gfx/VertexArray.h"
#include "gfx/RenderCommand.h"
#include "Renderer.h"
#include "sprite/Sprite.h"
#include "font/Font.h"
#include "core/profiler/CPUGPUProfiler.h"
#include "TextShader.h"

namespace BHive
{

	struct TextVertex
	{
		glm::vec4 Position;
		glm::vec2 TexCoord;
		glm::vec4 Color;
		uint32_t Texture;
		glm::vec2 Thickness;
		glm::vec2 Outline;
		glm::vec4 OutlineColor;

		static BufferLayout Layout()
		{
			return {{EShaderDataType::Float4}, {EShaderDataType::Float2}, {EShaderDataType::Float4}, {EShaderDataType::Int},
					{EShaderDataType::Float2}, {EShaderDataType::Float2}, {EShaderDataType::Float4}};
		}
	};

	struct TextRenderer::RenderData
	{
		static const uint32_t sMaxQuads = 20'000;
		static const uint32_t sMaxVertices = sMaxQuads * 4;
		static const uint32_t sMaxIndices = sMaxQuads * 6;

		TextVertex *mVertexCurrentPtr = nullptr;
		TextVertex *mVertexBufferPtr = nullptr;

		uint32_t *mIndexCurrentPtr = nullptr;
		uint32_t *mIndexBufferPtr = nullptr;

		Ref<IndexBuffer> mIndexBuffer;
		Ref<VertexBuffer> mVertexBuffer;
		Ref<VertexArray> mVertexArray;

		Ref<Shader> mTextShader;

		uint32_t mIndexCount = 0;
		uint32_t mVertexCount = 0;
		uint32_t mTextureCount = 1;

		std::array<Ref<Texture>, 32> mTextures;

		RenderData()
		{
			mVertexBufferPtr = new TextVertex[sMaxVertices];
			mIndexBufferPtr = new uint32_t[sMaxIndices];

			mIndexBuffer = IndexBuffer::Create(sMaxIndices);
			mVertexBuffer = VertexBuffer::Create(sMaxVertices * sizeof(TextVertex));
			mVertexBuffer->SetLayout(TextVertex::Layout());

			mVertexArray = VertexArray::Create();
			mVertexArray->SetIndexBuffer(mIndexBuffer);
			mVertexArray->AddVertexBuffer(mVertexBuffer);

			mTextShader = ShaderLibrary::Load("Text", text_vert, text_frag);

			mTextures[0] = Renderer::GetWhiteTexture();
		}

		~RenderData()
		{
			delete[] mVertexBufferPtr;
			delete[] mIndexBufferPtr;
		}
	};

	void TextRenderer::Init()
	{
		sData = new RenderData();
	}

	void TextRenderer::Shutdown()
	{
		delete sData;
	}

	void TextRenderer::Begin()
	{
		StartBatch();
	}

	void TextRenderer::End()
	{
		Flush();
	}

	void TextRenderer::DrawText(const Ref<Font> &font, float size_arg, const std::string &text, const FontParams &params, const FTransform &transform)
	{
		auto texture = font->GetAtlas();
		auto scale = font->GetDeviceScale() * size_arg;
		auto line_height = font->GetLineHeight() * scale;

		glm::vec2 texel_size = 1.0f / glm::vec2{texture->GetWidth(), texture->GetHeight()};

		glm::vec2 coords[4];

		glm::vec2 offset = {};
		for (auto &c : text)
		{

			auto &character = font->GetCharacter(c);
			const auto &bounds = character.Bounds;
			const auto &pos = character.Position;
			const auto &size = character.Size;
			const auto &bearing = character.Bearing;
			auto advance = character.Advance >> 6;

			float x = offset.x + (bearing.x * scale);
			float y = offset.y - (size.y - bearing.y) * scale;
			float w = size.x * scale;
			float h = size.y * scale;

			glm::vec3 quad[4];
			quad[0] = {x, y, 0};
			quad[1] = {x + w, y, 0};
			quad[2] = {x + w, y + h, 0};
			quad[3] = {x, y + h, 0};

			coords[1] = (pos + size) * texel_size;
			coords[0] = glm::vec2{pos.x, pos.y + size.y} * texel_size;
			coords[3] = pos * texel_size;
			coords[2] = glm::vec2{pos.x + size.x, pos.y} * texel_size;

			if (c == '\n')
			{
				offset.x = 0.f;
				offset.y -= (line_height + params.LineSpacing);
				continue;
			}

			if (c == '\t')
			{
				offset.x += 4.0f * (scale * advance + params.Kerning);
				continue;
			}

			DrawQuad(quad, coords, {1, 1}, params.Style, transform, texture);

			// bitshift advance to get value in pixels (2^6 = 64)
			offset.x += scale * advance + params.Kerning;
		}
	}

	void TextRenderer::DrawQuad(
		const glm::vec3 *points, const glm::vec2 *texcoords, const glm::vec2 &size, const FontStyle &style, const glm::mat4 &transform,
		const Ref<Texture> &texture)
	{
		static uint32_t indices[] = {0, 1, 2, 2, 3, 0};

		if (sData->mVertexCount >= TextRenderer::RenderData::sMaxVertices)
		{
			NextBatch();
		}

		uint32_t texture_index = 0;
		for (uint32_t t = 0; t < sData->mTextureCount; t++)
		{
			if (texture && *sData->mTextures[t] == *texture)
			{
				texture_index = t;
				break;
			}
		}

		if (texture_index == 0 && texture)
		{
			if (sData->mTextureCount >= 32)
				NextBatch();

			texture_index = sData->mTextureCount;
			sData->mTextures[sData->mTextureCount] = texture;
			sData->mTextureCount++;
		}

		for (uint32_t i = 0; i < 4; i++)
		{
			sData->mVertexCurrentPtr->Position = transform * (glm::vec4(points[i], 1.0f) * glm::vec4(size, 1.f, 1.f));
			sData->mVertexCurrentPtr->TexCoord = texcoords[i];
			sData->mVertexCurrentPtr->Color = style.TextColor;
			sData->mVertexCurrentPtr->Thickness = {style.Thickness, style.Smoothness};
			sData->mVertexCurrentPtr->Outline = {style.OutlineThickness, style.OutlineSmoothness};
			sData->mVertexCurrentPtr->OutlineColor = style.OutlineColor;
			sData->mVertexCurrentPtr->Texture = texture_index;
			sData->mVertexCurrentPtr++;
		}

		auto offset = sData->mVertexCount;

		for (uint32_t i = 0; i < 6; i++)
		{
			*sData->mIndexCurrentPtr = indices[i] + offset;
			sData->mIndexCurrentPtr++;
		}

		sData->mVertexCount += 4;
		sData->mIndexCount += 6;
	}

	void TextRenderer::StartBatch()
	{
		sData->mIndexCount = 0;
		sData->mVertexCount = 0;
		sData->mTextureCount = 1;

		sData->mVertexCurrentPtr = sData->mVertexBufferPtr;
		sData->mIndexCurrentPtr = sData->mIndexBufferPtr;
	}

	void TextRenderer::NextBatch()
	{
		Flush();
		StartBatch();
	}

	void TextRenderer::Flush()
	{
		GPU_PROFILER_FUNCTION();

		if (sData->mIndexCount)
		{
			{
				uint32_t size = (uint32_t)((uint8_t *)sData->mVertexCurrentPtr - (uint8_t *)sData->mVertexBufferPtr);
				sData->mVertexBuffer->SetData(sData->mVertexBufferPtr, size);
			}

			{
				uint32_t size = (uint32_t)((uint8_t *)sData->mIndexCurrentPtr - (uint8_t *)sData->mIndexBufferPtr);
				sData->mIndexBuffer->SetData(sData->mIndexBufferPtr, size);
			}

			sData->mTextShader->Bind();

			for (uint32_t i = 0; i < sData->mTextureCount; i++)
			{
				if (auto texture = sData->mTextures[i])
				{
					texture->Bind(i);
				}
			}

			RenderCommand::DrawElements(Triangles, *sData->mVertexArray, sData->mIndexCount);
			Renderer::GetStats().DrawCalls++;

			sData->mTextShader->UnBind();
		}
	}

	TextRenderer::RenderData *TextRenderer::sData = nullptr;
} // namespace BHive