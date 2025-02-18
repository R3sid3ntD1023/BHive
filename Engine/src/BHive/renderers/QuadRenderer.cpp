#include "QuadRenderer.h"
#include "gfx/Shader.h"
#include "gfx/Texture.h"
#include "gfx/VertexArray.h"
#include "gfx/RenderCommand.h"
#include "Renderer.h"
#include "sprite/Sprite.h"
#include "font/Font.h"
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

	struct QuadRenderer::RenderData
	{
		static const uint32_t sMaxQuads = 20'000;
		static const uint32_t sMaxVertices = sMaxQuads * 4;
		static const uint32_t sMaxIndices = sMaxQuads * 6;

		QuadVertex *mVertexCurrentPtr = nullptr;
		QuadVertex *mVertexBufferPtr = nullptr;

		uint32_t *mIndexCurrentPtr = nullptr;
		uint32_t *mIndexBufferPtr = nullptr;

		Ref<IndexBuffer> mIndexBuffer;
		Ref<VertexBuffer> mVertexBuffer;
		Ref<VertexArray> mVertexArray;

		Ref<Shader> mQuadShader;

		uint32_t mIndexCount = 0;
		uint32_t mVertexCount = 0;
		uint32_t mTextureCount = 1;

		std::array<Ref<Texture>, 32> mTextures;

		glm::mat4 mCameraView{1.f};

		RenderData()
		{
			mVertexBufferPtr = new QuadVertex[sMaxVertices];
			mIndexBufferPtr = new uint32_t[sMaxIndices];

			mIndexBuffer = IndexBuffer::Create(sMaxIndices);
			mVertexBuffer = VertexBuffer::Create(sMaxVertices * sizeof(QuadVertex));
			mVertexBuffer->SetLayout(QuadVertex::Layout());

			mVertexArray = VertexArray::Create();
			mVertexArray->SetIndexBuffer(mIndexBuffer);
			mVertexArray->AddVertexBuffer(mVertexBuffer);

			mQuadShader = ShaderLibrary::Load(ENGINE_PATH "/data/shaders/Quad.glsl");

			mTextures[0] = Renderer::GetWhiteTexture();
		}

		~RenderData()
		{
			delete[] mVertexBufferPtr;
			delete[] mIndexBufferPtr;
		}
	};

	void QuadRenderer::Init()
	{
		sData = new RenderData();
	}

	void QuadRenderer::Shutdown()
	{
		delete sData;
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

	void QuadRenderer::DrawQuad(
		const glm::vec2 &size, const Color &color, const FTransform &transform, const Ref<Texture> &texture, QuadRendererFlags flags)
	{
		static glm::vec3 positions[4] = {{-.5f, -.5f, 0.f}, {.5f, -.5f, 0.f}, {.5f, .5f, 0.f}, {-.5f, .5f, 0.f}};

		static glm::vec2 texcoords[4] = {{0.f, 0.f}, {1.f, 0.f}, {1.f, 1.f}, {0.f, 1.f}};

		DrawQuad(positions, texcoords, size, color, transform.to_mat4(), texture, flags);
	}

	void QuadRenderer::DrawSprite(
		const glm::vec2 &size, const Color &color, const FTransform &transform, const Ref<Sprite> &sprite, QuadRendererFlags flags)
	{
		if (!sprite)
			return;

		static glm::vec3 positions[4] = {{-.5f, -.5f, 0.f}, {.5f, -.5f, 0.f}, {.5f, .5f, 0.f}, {-.5f, .5f, 0.f}};

		DrawQuad(positions, sprite->GetCoords(), size, color, transform.to_mat4(), sprite->GetSourceTexture().get(), flags);
	}

	void QuadRenderer::DrawBillboard(
		const glm::vec2 &size, const Color &color, const FTransform &transform, const Ref<Texture> &texture, QuadRendererFlags flags)
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
		const glm::vec3 *points, const glm::vec2 *texcoords, const glm::vec2 &size, const Color &color, const glm::mat4 &transform,
		const Ref<Texture> &texture, QuadRendererFlags flags)
	{
		static uint32_t indices[] = {0, 1, 2, 2, 3, 0};

		if (sData->mVertexCount >= QuadRenderer::RenderData::sMaxVertices)
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

	void QuadRenderer::StartBatch()
	{
		sData->mIndexCount = 0;
		sData->mVertexCount = 0;
		sData->mTextureCount = 1;

		sData->mVertexCurrentPtr = sData->mVertexBufferPtr;
		sData->mIndexCurrentPtr = sData->mIndexBufferPtr;
	}

	void QuadRenderer::NextBatch()
	{
		Flush();
		StartBatch();
	}

	void QuadRenderer::Flush()
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

			sData->mQuadShader->Bind();

			for (uint32_t i = 0; i < sData->mTextureCount; i++)
			{
				if (auto texture = sData->mTextures[i])
				{
					texture->Bind(i);
				}
			}

			RenderCommand::DrawElements(Triangles, *sData->mVertexArray, sData->mIndexCount);

			Renderer::GetStats().DrawCalls++;

			sData->mQuadShader->UnBind();
		}
	}

	QuadRenderer::RenderData *QuadRenderer::sData = nullptr;
} // namespace BHive