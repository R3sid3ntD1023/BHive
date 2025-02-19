#include "QuadRenderer.h"
#include "gfx/Shader.h"
#include "gfx/Texture.h"
#include "gfx/VertexArray.h"
#include "gfx/RenderCommand.h"
#include "Renderer.h"
#include "sprite/Sprite.h"
#include "font/Font.h"
#include "font/FontManager.h"
#include "shaders/TextShader.h"
#include "shaders/CircleShader.h"
#include "core/profiler/CPUGPUProfiler.h"

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
		uint32_t mTextureCount = 1;

		std::array<Ref<Texture>, 32> mTextures;

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

			mIndexBuffer = IndexBuffer::Create(RenderData2D::sMaxIndices);
			mVertexBuffer = VertexBuffer::Create(RenderData2D::sMaxVertices * sizeof(QuadVertex));
			mVertexBuffer->SetLayout(QuadVertex::Layout());

			mVertexArray = VertexArray::Create();
			mVertexArray->SetIndexBuffer(mIndexBuffer);
			mVertexArray->AddVertexBuffer(mVertexBuffer);

			mShader = ShaderLibrary::Load(ENGINE_PATH "/data/shaders/Quad.glsl");
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

				for (uint32_t i = 0; i < mTextures.mTextureCount; i++)
				{
					if (auto texture = mTextures.mTextures[i])
					{
						texture->Bind(i);
					}
				}

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

			mIndexBuffer = IndexBuffer::Create(RenderData2D::sMaxIndices);
			mVertexBuffer = VertexBuffer::Create(RenderData2D::sMaxVertices * sizeof(TextVertex));
			mVertexBuffer->SetLayout(TextVertex::Layout());

			mVertexArray = VertexArray::Create();
			mVertexArray->SetIndexBuffer(mIndexBuffer);
			mVertexArray->AddVertexBuffer(mVertexBuffer);

			mShader = ShaderLibrary::Load("Text", text_vert, text_frag);
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

				for (uint32_t i = 0; i < mTextures.mTextureCount; i++)
				{
					if (auto texture = mTextures.mTextures[i])
					{
						texture->Bind(i);
					}
				}

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

			mIndexBuffer = IndexBuffer::Create(RenderData2D::sMaxIndices);
			mVertexBuffer = VertexBuffer::Create(RenderData2D::sMaxVertices * sizeof(CircleVertex));
			mVertexBuffer->SetLayout(CircleVertex::Layout());

			mVertexArray = VertexArray::Create();
			mVertexArray->SetIndexBuffer(mIndexBuffer);
			mVertexArray->AddVertexBuffer(mVertexBuffer);

			mShader = ShaderLibrary::Load("Circle", circle_vert, circle_frag);
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

	void QuadRenderer::Begin(const glm::mat4 &view)
	{
		sData->mCameraView = view;
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

	void QuadRenderer::DrawQuad(
		const glm::vec2 &size, const Color &color, const FTransform &transform, const Ref<Texture> &texture,
		QuadRendererFlags flags)
	{
		static glm::vec3 positions[4] = {{-.5f, -.5f, 0.f}, {.5f, -.5f, 0.f}, {.5f, .5f, 0.f}, {-.5f, .5f, 0.f}};

		static glm::vec2 texcoords[4] = {{0.f, 0.f}, {1.f, 0.f}, {1.f, 1.f}, {0.f, 1.f}};

		DrawQuad(positions, texcoords, size, color, transform.to_mat4(), texture, flags);
	}

	void QuadRenderer::DrawSprite(
		const glm::vec2 &size, const Color &color, const FTransform &transform, const Ref<Sprite> &sprite,
		QuadRendererFlags flags)
	{
		if (!sprite)
			return;

		static glm::vec3 positions[4] = {{-.5f, -.5f, 0.f}, {.5f, -.5f, 0.f}, {.5f, .5f, 0.f}, {-.5f, .5f, 0.f}};

		DrawQuad(positions, sprite->GetCoords(), size, color, transform.to_mat4(), sprite->GetSourceTexture().get(), flags);
	}

	void QuadRenderer::DrawBillboard(
		const glm::vec2 &size, const Color &color, const FTransform &transform, const Ref<Texture> &texture,
		QuadRendererFlags flags)
	{
		const auto &view = sData->mCameraView;
		glm::vec3 positions[4] = {{-.5f, -.5f, 0.f}, {.5f, -.5f, 0.f}, {.5f, .5f, 0.f}, {-.5f, .5f, 0.f}};

		const static glm::vec2 texcoords[4] = {{0.f, 0.f}, {1.f, 0.f}, {1.f, 1.f}, {0.f, 1.f}};

		const glm::vec3 camera_right = glm::vec3{view[0][0], view[1][0], view[2][0]};
		const glm::vec3 camera_up = glm::vec3{view[0][1], view[1][1], view[2][1]};
		for (uint32_t i = 0; i < 4; i++)
		{
			auto newposition = glm::vec4(positions[i] * glm::vec3(size, 1), 1.0f);
			glm::vec3 world_space_center = transform.to_mat4() * glm::vec4(0.0f, 0.f, 0.f, 1.f);
			positions[i] = world_space_center + camera_right * newposition.x + camera_up * newposition.y;
		}

		DrawQuad(positions, texcoords, size, color, {1.f}, texture, flags);
	}

	void QuadRenderer::DrawQuad(
		const glm::vec3 *points, const glm::vec2 *texcoords, const glm::vec2 &size, const Color &color,
		const glm::mat4 &transform, const Ref<Texture> &texture, QuadRendererFlags flags)
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
			sData->mVertexCurrentPtr->TexCoord = texcoords[i];
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

			DrawTextQuad(quad, coords, {1, 1}, params.Style, transform, texture);

			// bitshift advance to get value in pixels (2^6 = 64)
			offset.x += scale * advance + params.Kerning;
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
			if (texture && *textures.mTextures[t] == *texture)
			{
				texture_index = t;
				break;
			}
		}

		if (texture_index == 0 && texture)
		{
			if (textures.mTextureCount >= 32)
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