#pragma once

#include "RenderBatch.h"
#include "VertexBatchBuffer.h"
#include "gfx/material/Material.h"

namespace BHive
{
	class Material;
	class Pipeline;

	struct FLineVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		int32_t EntityID = -1;

		static BufferLayout GetLayout() { return {{EShaderDataType::Float3}, {EShaderDataType::Float4}, {EShaderDataType::Int}};}
	};

	struct LineRenderBatch : public IRenderBatch
	{
		const static uint32_t sMaxVertexCount = 20'000;

		void Initialize() override;

		bool NeedsFlush(uint32_t vNeeded, uint32_t iNeeded) override;

		void StartBatch() override;

		void Flush(Renderer& renderer) override;

		bool IsActive() const override { return mIsActive; }

		VertexBatchBuffer<FLineVertex> &GetBuffer() { return *mBuffer; }

	private:
		bool IsFull(uint32_t vNeeded, uint32_t iNeeded);

	private:
		Scope<VertexBatchBuffer<FLineVertex>> mBuffer;
		Scope<Material> mLineMaterial;
		bool mIsActive = false;
	};
} // namespace BHive