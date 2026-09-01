#pragma once

#include "RenderBatch.h"
#include "VertexBatchBuffer.h"
#include "gfx/material/Material.h"

namespace BHive
{
	struct CircleVertex
	{
		glm::vec4 WorldPosition{};
		glm::vec3 LocalPosition{};
		glm::vec4 Color{};
		float Thickness{};
		float Fade{};
		int32_t EntityID{-1};

		static BufferLayout GetLayout()
		{
			return {{EShaderDataType::Float4}, {EShaderDataType::Float3}, {EShaderDataType::Float4}, {EShaderDataType::Float}, {EShaderDataType::Float}, {EShaderDataType::Int}};
		}
	};

	struct CircleRenderBatch final : public IRenderBatch
	{
		static const uint32_t sMaxQuads = 20'000;
		static const uint32_t sMaxVertexCount = sMaxQuads * 4;
		static const uint32_t sMaxIndexCount = sMaxQuads * 6;

		void Initialize() override;

		void StartBatch() override;

		bool NeedsFlush(uint32_t vNeeded, uint32_t iNeeded) override;

		void Flush(Renderer &renderer) override;

		bool IsActive() const override { return mIsActive; }

		VertexBatchBuffer<CircleVertex> &GetBuffer() { return *mBuffer; }

	private:
		bool IsFull(uint32_t vNeeded, uint32_t iNeeded);

	private:
		Scope<VertexBatchBuffer<CircleVertex>> mBuffer;
		MaterialPtr mCircleMaterial;
		PipelinePtr mPipeline;
		bool mIsActive = false;
	};
} // namespace BHive