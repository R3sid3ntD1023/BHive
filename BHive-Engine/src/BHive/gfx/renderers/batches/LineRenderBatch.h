#pragma once

#include "gfx/Buffers.h"
#include "gfx/VertexArray.h"
#include "RenderBatch.h"

namespace BHive
{
	class Material;
	class Pipeline;

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

		FLineVertex *operator->();

		LineRenderBatch &operator++(int);

		~LineRenderBatch();

	private:
		Ref<ShaderProgram> mLineShader;
		Ref<VertexBuffer> mVertexBuffer;
		Ref<VertexArray> mVertexArray;

		FLineVertex *mVertexDataBuffer = nullptr;
		FLineVertex *mVertexDataPtr = nullptr;
		uint32_t mVertexCount = 0;

		Ref<Material> mLineMaterial;
		Ref<Pipeline> mPipeline;
	};
} // namespace BHive