#include "BillboardRenderer.h"
#include "gfx/VertexArray.h"
#include "gfx/Shader.h"
#include "Renderer.h"
#include "gfx/Texture.h"
#include "gfx/RenderCommand.h"

namespace BHive
{
	struct BillboardVertex
	{
		glm::vec4 Position;
		glm::vec2 TexCoord;
		glm::vec4 Color;
		uint32_t Texture;

		static BufferLayout Layout()
		{
			return {
				{EShaderDataType::Float4},
				{EShaderDataType::Float2},
				{EShaderDataType::Float4},
				{EShaderDataType::Int},
			};
		}
	};

	struct BillboardRenderer::RenderData
	{
		static const uint32_t sMaxQuads = 20'000;
		static const uint32_t sMaxVertices = sMaxQuads * 4;
		static const uint32_t sMaxIndices = sMaxQuads * 6;

		BillboardVertex* mVertexCurrentPtr = nullptr;
		BillboardVertex* mVertexBufferPtr = nullptr;

		uint32_t* mIndexCurrentPtr = nullptr;
		uint32_t* mIndexBufferPtr = nullptr;

		Ref<IndexBuffer>	mIndexBuffer;
		Ref<VertexBuffer>	mVertexBuffer;
		Ref<VertexArray>	mVertexArray;

		Ref<Shader> mBillboardShader;

		uint32_t mIndexCount = 0;
		uint32_t mVertexCount = 0;
		uint32_t mTextureCount = 1;

		std::array<Ref<Texture>, 32> mTextures;

		RenderData()
		{
			mVertexBufferPtr = new BillboardVertex[sMaxVertices];
			mIndexBufferPtr = new uint32_t[sMaxIndices];

			mIndexBuffer = IndexBuffer::Create(sMaxIndices);
			mVertexBuffer = VertexBuffer::Create(sMaxVertices * sizeof(BillboardVertex));
			mVertexBuffer->SetLayout(BillboardVertex::Layout());

			mVertexArray = VertexArray::Create();
			mVertexArray->SetIndexBuffer(mIndexBuffer);
			mVertexArray->AddVertexBuffer(mVertexBuffer);

			mBillboardShader = ShaderLibrary::Load(ENGINE_PATH"/data/shaders/Billboard.glsl");
			mTextures[0] = Renderer::GetWhiteTexture();
		}

		~RenderData()
		{
			delete[] mVertexBufferPtr;
			delete[] mIndexBufferPtr;
		}
	};

	void BillboardRenderer::Init()
	{
		sData = new RenderData();
	}

	void BillboardRenderer::Shutdown()
	{
		delete sData;
	}

	void BillboardRenderer::Begin()
	{
		StartBatch();
	}

	void BillboardRenderer::End()
	{
		Flush();
	}

	void BillboardRenderer::DrawBillboard(const glm::vec2& size, const Color& color, const FTransform& transform, const Ref<Texture>& texture)
	{
		glm::vec3 positions[4] =
		{
			{-.5f, -.5f, 0.f},
			{.5f, -.5f, 0.f},
			{.5f, .5f, 0.f},
			{-.5f, .5f, 0.f}
		};

		static glm::vec2 texcoords[4] =
		{
			{0.f, 0.f},
			{1.f, 0.f},
			{1.f, 1.f},
			{0.f, 1.f}
		};
		static uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };
		

		if (sData->mVertexCount >= BillboardRenderer::RenderData::sMaxVertices)
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

		if (texture_index == 0)
		{
			if (sData->mTextureCount >= 32)
				NextBatch();

			auto& texture_count = sData->mTextureCount;
			texture_index = texture_count;
			sData->mTextures[texture_count % 32] = texture;
			texture_count++;
		}

		const auto& view = Renderer::GetView();
		glm::vec3 camera_right = glm::vec3{ view[0][0], view[1][0], view[2][0] };
		glm::vec3 camera_up = glm::vec3{ view[0][1], view[1][1], view[2][1] };
		for (uint32_t i = 0; i < 4; i++)
		{
			auto newposition = glm::vec4(positions[i] * glm::vec3(size, 1), 1.0f);
			glm::vec3 world_space_center = transform.to_mat4() * glm::vec4(0.0f, 0.f, 0.f, 1.f);
			positions[i] = world_space_center + camera_right * newposition.x +
				camera_up * newposition.y;
		}

		for (uint32_t i = 0; i < 4; i++)
		{
			sData->mVertexCurrentPtr->Position =  glm::vec4(positions[i], 1);
			sData->mVertexCurrentPtr->TexCoord = texcoords[i];
			sData->mVertexCurrentPtr->Color = color;
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

	void BillboardRenderer::StartBatch()
	{
		sData->mIndexCount = 0;
		sData->mVertexCount = 0;
		sData->mTextureCount = 1;

		sData->mVertexCurrentPtr = sData->mVertexBufferPtr;
		sData->mIndexCurrentPtr = sData->mIndexBufferPtr;
	}

	void BillboardRenderer::NextBatch()
	{
		Flush();
		StartBatch();
	}

	void BillboardRenderer::Flush()
	{
		if (sData->mIndexCount)
		{
			{
				uint32_t size = (uint32_t)((uint8_t*)sData->mVertexCurrentPtr - (uint8_t*)sData->mVertexBufferPtr);
				sData->mVertexBuffer->SetData(sData->mVertexBufferPtr, size);
			}

			{
				uint32_t size = (uint32_t)((uint8_t*)sData->mIndexCurrentPtr - (uint8_t*)sData->mIndexBufferPtr);
				sData->mIndexBuffer->SetData(sData->mIndexBufferPtr, size);
			}

			sData->mBillboardShader->Bind();

			for (uint32_t i = 0; i < sData->mTextureCount; i++)
			{
				if (auto texture = sData->mTextures[i])
				{
					texture->Bind(i);
				}
			}

			RenderCommand::DrawElements(Triangles, *sData->mVertexArray, sData->mIndexCount);
			Renderer::GetStats().DrawCalls++;

			sData->mBillboardShader->UnBind();
		}
	}


	BillboardRenderer::RenderData* BillboardRenderer::sData = nullptr;
}