#pragma once

#include "gfx/Buffers.h"
#include "gfx/VertexArray.h"
#include "gfx/VulkanCore.h"
#include "RenderBatch.h"

namespace BHive
{
	class VulkanPipeline;
	class FDescriptorSetLayout;
	class FDescriptorPool;

	struct FLineVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		int32_t EntityID = -1;
	};

	struct LineRenderBatch : public IRenderBatch
	{
		const static uint32_t sMaxVertexCount = 20'000;

		void Init();

		void End() override;

		void NextBatch() override;

		void StartBatch() override;

		void Flush() override;

		FLineVertex *operator->() { return mVertexDataPtr; }

		LineRenderBatch &operator++(int)
		{
			mVertexDataPtr++;
			mVertexCount++;
			return *this;
		}

		~LineRenderBatch();

	private:
		Ref<Shader> mLineShader;
		Ref<VertexBuffer> mVertexBuffer;
		Ref<VertexArray> mVertexArray;

		FLineVertex *mVertexDataBuffer = nullptr;
		FLineVertex *mVertexDataPtr = nullptr;
		uint32_t mVertexCount = 0;

		Ref<VulkanPipeline> mGraphicsPipeline = nullptr;

		vk::raii::PipelineLayout mPipelineLayout = nullptr;

		Ref<FDescriptorSetLayout> mDescriptorSetLayout;
		Ref<FDescriptorPool> mDescriptorPool;
		vk::raii::DescriptorSets mDescriptorSets = nullptr;
	};
} // namespace BHive