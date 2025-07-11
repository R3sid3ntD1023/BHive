#include "core/profiler/CPUGPUProfiler.h"
#include "font/Font.h"
#include "font/FontManager.h"
#include "font/MSDFData.h"
#include "gfx/RenderCommand.h"
#include "gfx/Shader.h"
#include "gfx/ShaderManager.h"
#include "gfx/StorageBuffer.h"
#include "gfx/textures/Texture2D.h"
#include "gfx/VertexArray.h"
#include "QuadRenderer.h"
#include "Renderer.h"
#include "renderers/Renderer.h"
#include "shaders/CircleShader.h"
#include "shaders/TextShader.h"
#include "shaders/QuadShader.h"
#include "sprite/Sprite.h"

namespace BHive
{
	struct QuadVertex
	{
		glm::vec4 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoord;
		glm::vec4 Color;
		uint32_t Texture;
		int Flags;

		static BufferLayout Layout()
		{
			return {{EShaderDataType::Float4}, {EShaderDataType::Float3}, {EShaderDataType::Float2},
					{EShaderDataType::Float4}, {EShaderDataType::Int},	  {EShaderDataType::Int}};
		}
	};

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

	struct CircleVertex
	{
		glm::vec4 WorldPosition;
		glm::vec3 LocalPosition;
		glm::vec4 Color;
		float Thickness;
		float Fade;

		static BufferLayout Layout()
		{
			return {
				{EShaderDataType::Float4},
				{EShaderDataType::Float3},
				{EShaderDataType::Float4},
				{EShaderDataType::Float},
				{EShaderDataType::Float}};
		}
	};

	struct TextureData
	{
		static inline const uint32_t sMaxTextureCount = 512;

		uint32_t mTextureCount = 1;

		std::array<Ref<Texture>, sMaxTextureCount> mTextures;

		TextureData() { mTextures[0] = Renderer::GetWhiteTexture(); }
	};

	struct IRenderDataBase
	{
		virtual void StartBatch() = 0;
		virtual void NextBatch() = 0;
		virtual void Flush() = 0;
	};

	template <typename T>
	struct TRenderData : public IRenderDataBase
	{
		uint32_t *mIndexCurrentPtr = nullptr;
		uint32_t *mIndexBufferPtr = nullptr;

		Ref<IndexBuffer> mIndexBuffer;
		Ref<VertexBuffer> mVertexBuffer;
		Ref<VertexArray> mVertexArray;

		// current vertex and index
		T *mVertexCurrentPtr = nullptr;
		T *mVertexBufferPtr = nullptr;

		uint32_t mIndexCount = 0;
		uint32_t mVertexCount = 0;

		virtual ~TRenderData()
		{
			delete[] mVertexBufferPtr;
			delete[] mIndexBufferPtr;
		}

		// sets the index and vertex buffer data
		virtual void Flush()
		{
			mVertexBuffer->SetData(mVertexBufferPtr, GetVertexBufferSize());
			mIndexBuffer->SetData(mIndexBufferPtr, GetIndexBufferSize());
		}

		virtual void StartBatch()
		{
			mIndexCount = 0;
			mVertexCount = 0;
			mVertexCurrentPtr = mVertexBufferPtr;
			mIndexCurrentPtr = mIndexBufferPtr;
		}

		virtual void NextBatch()
		{
			Flush();
			StartBatch();
		}

		size_t GetVertexBufferSize() { return (size_t)((uint8_t *)mVertexCurrentPtr - (uint8_t *)mVertexBufferPtr); }
		size_t GetIndexBufferSize() { return (size_t)((uint8_t *)mIndexCurrentPtr - (uint8_t *)mIndexBufferPtr); }
	};

	struct RenderData2D
	{
		static const uint32_t sMaxQuads = 20'000;
		static const uint32_t sMaxVertices = sMaxQuads * 4;
		static const uint32_t sMaxIndices = sMaxQuads * 6;
	};

	struct QuadRenderer::RenderData : public TRenderData<QuadVertex>
	{
		TextureData mTextures;

		Ref<Shader> mShader;

		glm::mat4 mCameraView{1.f};

		RenderData()
		{
			mVertexBufferPtr = new QuadVertex[RenderData2D::sMaxVertices];
			mIndexBufferPtr = new uint32_t[RenderData2D::sMaxIndices];

			mIndexBuffer = CreateRef<IndexBuffer>(RenderData2D::sMaxIndices);
			mVertexBuffer = CreateRef<VertexBuffer>(RenderData2D::sMaxVertices * sizeof(QuadVertex));
			mVertexBuffer->SetLayout(QuadVertex::Layout());

			mVertexArray = CreateRef<VertexArray>();
			mVertexArray->SetIndexBuffer(mIndexBuffer);
			mVertexArray->AddVertexBuffer(mVertexBuffer);

			mShader = ShaderManager::Get().Load("Quad", quad_vert, quad_frag);
		}

		void StartBatch() override
		{
			TRenderData::StartBatch();
			mTextures.mTextureCount = 1;
		}

		void Flush() override
		{
			if (mIndexCount)
			{
				TRenderData::Flush();

				mShader->Bind();

				for (size_t i = 0; i < mTextures.mTextureCount; i++)
					mTextures.mTextures[i]->Bind(i);

				RenderCommand::DrawElements(Triangles, *mVertexArray, mIndexCount);

				Renderer::GetStats().DrawCalls++;

				mShader->UnBind();
			}
		}
	};

	struct QuadRenderer::TextData : public TRenderData<TextVertex>
	{
		TextureData mTextures;

		Ref<Shader> mShader;

		TextData()
		{
			mVertexBufferPtr = new TextVertex[RenderData2D::sMaxVertices];
			mIndexBufferPtr = new uint32_t[RenderData2D::sMaxIndices];

			mIndexBuffer = CreateRef<IndexBuffer>(RenderData2D::sMaxIndices);
			mVertexBuffer = CreateRef<VertexBuffer>(RenderData2D::sMaxVertices * sizeof(TextVertex));
			mVertexBuffer->SetLayout(TextVertex::Layout());

			mVertexArray = CreateRef<VertexArray>();
			mVertexArray->SetIndexBuffer(mIndexBuffer);
			mVertexArray->AddVertexBuffer(mVertexBuffer);
			mShader = mShader = ShaderManager::Get().Load("Text", text_vert, text_frag);
		}

		void StartBatch() override
		{
			TRenderData::StartBatch();
			mTextures.mTextureCount = 1;
		}

		void Flush() override
		{
			if (mIndexCount)
			{
				TRenderData::Flush();

				mShader->Bind();

				for (size_t i = 0; i < mTextures.mTextureCount; i++)
					mTextures.mTextures[i]->Bind(i);

				RenderCommand::EnableDepthMask(false);
				RenderCommand::DrawElements(Triangles, *mVertexArray, mIndexCount);
				RenderCommand::EnableDepthMask(true);

				Renderer::GetStats().DrawCalls++;

				mShader->UnBind();
			}
		}
	};

	struct QuadRenderer::CircleData : public TRenderData<CircleVertex>
	{
		Ref<Shader> mShader;

		CircleData()
		{
			mVertexBufferPtr = new CircleVertex[RenderData2D::sMaxVertices];
			mIndexBufferPtr = new uint32_t[RenderData2D::sMaxIndices];

			mIndexBuffer = CreateRef<IndexBuffer>(RenderData2D::sMaxIndices);
			mVertexBuffer = CreateRef<VertexBuffer>(RenderData2D::sMaxVertices * sizeof(CircleVertex));
			mVertexBuffer->SetLayout(CircleVertex::Layout());

			mVertexArray = CreateRef<VertexArray>();
			mVertexArray->SetIndexBuffer(mIndexBuffer);
			mVertexArray->AddVertexBuffer(mVertexBuffer);

			mShader = ShaderManager::Get().Load("Circle", circle_vert, circle_frag);
		}

		void Flush() override
		{
			if (mIndexCount)
			{
				TRenderData::Flush();

				mShader->Bind();

				RenderCommand::DrawElements(Triangles, *mVertexArray, mIndexCount);

				Renderer::GetStats().DrawCalls++;

				mShader->UnBind();
			}
		}
	};

	void QuadRenderer::Init()
	{
		sData = new RenderData();
		sTextData = new TextData();
		sCircleData = new CircleData();
	}

	void QuadRenderer::Shutdown()
	{
		delete sData;
		delete sTextData;
		delete sCircleData;
	}

	void QuadRenderer::Begin()
	{
		sData->mCameraView = CameraBuffer::Get().GetCameraData().View;
		StartBatch();
	}

	void QuadRenderer::End()
	{
		Flush();
	}

	void QuadRenderer::DrawCircle(const FCircleParams &params, const FTransform &transform)
	{
		static glm::vec3 positions[4] = {{-.5f, -.5f, 0.f}, {.5f, -.5f, 0.f}, {.5f, .5f, 0.f}, {-.5f, .5f, 0.f}};

		static uint32_t indices[] = {0, 1, 2, 2, 3, 0};

		if (sCircleData->mIndexCount >= RenderData2D::sMaxIndices)
			sCircleData->NextBatch();

		for (int i = 0; i < 4; i++)
		{
			sCircleData->mVertexCurrentPtr->WorldPosition =
				transform.to_mat4() * glm::vec4(positions[i] * params.Radius, 1.f);
			sCircleData->mVertexCurrentPtr->LocalPosition = positions[i] * 2.f;
			sCircleData->mVertexCurrentPtr->Color = params.LineColor;
			sCircleData->mVertexCurrentPtr->Thickness = params.Thickness;
			sCircleData->mVertexCurrentPtr->Fade = params.Fade;
			sCircleData->mVertexCurrentPtr++;
		}

		auto offset = sCircleData->mVertexCount;
		for (int i = 0; i < 6; i++)
		{
			*sCircleData->mIndexCurrentPtr = offset + indices[i];
			sCircleData->mIndexCurrentPtr++;
		}

		sCircleData->mVertexCount += 4;
		sCircleData->mIndexCount += 6;
	}

	void QuadRenderer::DrawQuad(const FQuadParams &params, const Ref<Texture> &texture, const FTransform &transform)
	{
		static glm::vec3 positions[4] = {{-.5f, -.5f, 0.f}, {.5f, -.5f, 0.f}, {.5f, .5f, 0.f}, {-.5f, .5f, 0.f}};

		static glm::vec2 texcoords[4] = {{0.f, 0.f}, {1.f, 0.f}, {1.f, 1.f}, {0.f, 1.f}};

		DrawQuad(positions, texcoords, params.Size, params.Color, transform.to_mat4(), texture, params.Tiling, params.Flags);
	}

	void QuadRenderer::DrawSprite(const FQuadParams &params, const Ref<Sprite> &sprite, const FTransform &transform)
	{
		if (!sprite)
			return;

		static glm::vec3 positions[4] = {{-.5f, -.5f, 0.f}, {.5f, -.5f, 0.f}, {.5f, .5f, 0.f}, {-.5f, .5f, 0.f}};

		DrawQuad(
			positions, sprite->GetCoords(), params.Size, params.Color, transform.to_mat4(), sprite->GetSourceTexture(),
			params.Tiling, params.Flags);
	}

	void QuadRenderer::DrawBillboard(const FQuadParams &params, const Ref<Texture> &texture, const FTransform &transform)
	{
		const auto &view = sData->mCameraView;
		glm::vec3 positions[4] = {{-.5f, -.5f, 0.f}, {.5f, -.5f, 0.f}, {.5f, .5f, 0.f}, {-.5f, .5f, 0.f}};

		const static glm::vec2 texcoords[4] = {{0.f, 0.f}, {1.f, 0.f}, {1.f, 1.f}, {0.f, 1.f}};

		const glm::vec3 camera_right = glm::vec3{view[0][0], view[1][0], view[2][0]};
		const glm::vec3 camera_up = glm::vec3{view[0][1], view[1][1], view[2][1]};
		for (uint32_t i = 0; i < 4; i++)
		{
			auto newposition = glm::vec4(positions[i] * glm::vec3(params.Size, 1), 1.0f);
			glm::vec3 world_space_center = transform.to_mat4() * glm::vec4(0.0f, 0.f, 0.f, 1.f);
			positions[i] = world_space_center + camera_right * newposition.x + camera_up * newposition.y;
		}

		DrawQuad(positions, texcoords, params.Size, params.Color, {1.f}, texture, params.Tiling, params.Flags);
	}

	void QuadRenderer::DrawQuad(
		const glm::vec3 *points, const glm::vec2 *texcoords, const glm::vec2 &size, const FColor &color,
		const glm::mat4 &transform, const Ref<Texture> &texture, const glm::vec2 &tiling, QuadRendererFlags flags)
	{
		static uint32_t indices[] = {0, 1, 2, 2, 3, 0};

		if (sData->mVertexCount >= RenderData2D::sMaxVertices)
		{
			sData->NextBatch();
		}

		uint32_t texture_index = GetTextureIndex(sData, sData->mTextures, texture);

		for (uint32_t i = 0; i < 4; i++)
		{
			sData->mVertexCurrentPtr->Position = transform * (glm::vec4(points[i], 1.0f) * glm::vec4(size, 1.f, 1.f));
			sData->mVertexCurrentPtr->Normal = glm::transpose(glm::inverse(transform)) * glm::vec4(0, 0, 1, 0);
			sData->mVertexCurrentPtr->TexCoord = texcoords[i] * tiling;
			sData->mVertexCurrentPtr->Color = color;
			sData->mVertexCurrentPtr->Texture = texture_index;
			sData->mVertexCurrentPtr->Flags = flags;
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

	void
	QuadRenderer::DrawText(float size_arg, const std::string &text, const FTextParams &params, const FTransform &transform)
	{
		auto font = FontManager::Get().GetDefaultFont();
		DrawText(font, size_arg, text, params, transform);
	}

	void QuadRenderer::DrawText(
		const Ref<Font> &font, float size_arg, const std::string &text, const FTextParams &params,
		const FTransform &transform)
	{
		if (!font)
			return;

		auto texture = font->GetAtlas();
		const auto &fontgeometry = font->GetMSDFData()->FontGeometry;
		const auto &metrics = fontgeometry.getMetrics();

		double scale = (1.0 / (metrics.ascenderY - metrics.descenderY)) * size_arg;
		const float spaceGlyphAdvance = fontgeometry.getGlyph(' ')->getAdvance();

		glm::vec2 texel_size = 1.0f / glm::vec2{texture->GetWidth(), texture->GetHeight()};

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

			DrawTextQuad(quad, coords, {1, 1}, params.Style, transform, texture);

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
		const glm::vec3 *points, const glm::vec2 *texcoords, const glm::vec2 &size, const FTextStyle &style,
		const glm::mat4 &transform, const Ref<Texture> &texture)
	{
		static uint32_t indices[] = {0, 1, 2, 2, 3, 0};

		if (sTextData->mVertexCount >= RenderData2D::sMaxVertices)
		{
			sTextData->NextBatch();
		}

		uint32_t texture_index = GetTextureIndex(sTextData, sTextData->mTextures, texture);

		for (uint32_t i = 0; i < 4; i++)
		{
			sTextData->mVertexCurrentPtr->Position = transform * (glm::vec4(points[i], 1.0f) * glm::vec4(size, 1.f, 1.f));
			sTextData->mVertexCurrentPtr->TexCoord = texcoords[i];
			sTextData->mVertexCurrentPtr->Color = style.TextColor;
			sTextData->mVertexCurrentPtr->Thickness = {style.Thickness, style.Smoothness};
			sTextData->mVertexCurrentPtr->Outline = {style.OutlineThickness, style.OutlineSmoothness};
			sTextData->mVertexCurrentPtr->OutlineColor = style.OutlineColor;
			sTextData->mVertexCurrentPtr->Texture = texture_index;
			sTextData->mVertexCurrentPtr++;
		}

		auto offset = sTextData->mVertexCount;

		for (uint32_t i = 0; i < 6; i++)
		{
			*sTextData->mIndexCurrentPtr = indices[i] + offset;
			sTextData->mIndexCurrentPtr++;
		}

		sTextData->mVertexCount += 4;
		sTextData->mIndexCount += 6;
	}

	uint32_t QuadRenderer::GetTextureIndex(IRenderDataBase *data, TextureData &textures, const Ref<Texture> &texture)
	{
		uint32_t texture_index = 0;
		for (uint32_t t = 0; t < textures.mTextureCount; t++)
		{
			if (texture && textures.mTextures[t] == texture)
			{
				texture_index = t;
				break;
			}
		}

		if (texture_index == 0 && texture)
		{
			if (textures.mTextureCount >= TextureData::sMaxTextureCount)
				data->NextBatch();

			texture_index = textures.mTextureCount;
			textures.mTextures[textures.mTextureCount] = texture;
			textures.mTextureCount++;
		}

		return texture_index;
	}

	void QuadRenderer::StartBatch()
	{
		sData->StartBatch();
		sTextData->StartBatch();
		sCircleData->StartBatch();
	}

	void QuadRenderer::Flush()
	{
		GPU_PROFILER_FUNCTION();

		sData->Flush();
		sTextData->Flush();
		sCircleData->Flush();
	}
} // namespace BHive