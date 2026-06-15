#pragma once

#include "TextureBatch.h"
#include "RenderBatch.h"
#include "VertexBatchBuffer.h"
#include "gfx/material/Material.h"

namespace BHive
{
	struct QuadVertex
	{
		glm::vec4 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoord;
		glm::vec4 Color;
		int32_t TextureIndex;
		int32_t Flags;
		int32_t EntityID{-1};

		static BufferLayout GetLayout()
		{
			return {{EShaderDataType::Float4}, {EShaderDataType::Float3}, {EShaderDataType::Float2}, {EShaderDataType::Float4}, {EShaderDataType::Int}, {EShaderDataType::Int}, {EShaderDataType::Int}};
		}
	};


	struct QuadRenderBatch : public IRenderBatch
	{
		static const uint32_t sMaxQuads = 20'000;
		static const uint32_t sMaxVertexCount = sMaxQuads * 4;
		static const uint32_t sMaxIndexCount = sMaxQuads * 6;

		void Initialize() override;

		bool NeedsFlush(uint32_t vNeeded, uint32_t iNeeded) override;

		void StartBatch() override;

		void Flush(Renderer& renderer) override;

		void SetTextureBatch(TextureBatchData *texture_batch);

		bool IsActive() const override { return mIsActive; }

		VertexBatchBuffer<QuadVertex> &GetBuffer() { return *mBuffer; }

	private:
		bool IsFull(uint32_t vNeeded, uint32_t iNeeded);

	private:
		TextureBatchData *mTextureBatch = nullptr;
		Scope<VertexBatchBuffer<QuadVertex>> mBuffer;
		Scope<Material> mQuadMaterial;
		bool mIsActive = false;
	};
} // namespace BHive