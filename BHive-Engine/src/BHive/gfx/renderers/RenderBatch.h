#pragma once

#include "RenderQueue.h"
#include "gfx/rendergraph/Pass.h"

namespace BHive
{
	struct MultiDrawIndirectCommand
	{
		uint32_t indexCount = 0;
		uint32_t instanceCount = 0;
		uint32_t firstIndex = 0;
		int32_t vertexOffset = 0;
		uint32_t firstInstance = 0;
	};

	struct alignas(16) ObjectData
	{
		glm::mat4 ModelMatrix{1.0f};				  // model matrix
		glm::vec4 CenterRadius{0.f, 0.0f, 0.0f, 0.f}; // bounding sphere center.xyz + radius
		uint32_t ID = 0;							  // which mesh this instance belongs to
		uint32_t BoneOffset = 0;					  // bone offset
		glm::vec4 debugColor;
	};

	struct RenderBatch
	{
		struct DrawBatch
		{
			uint32_t FirstCommand = 0;
			uint32_t CommandCount = 0;
		};

		// vao -> material[submissions]
		std::unordered_map<VertexArrayPtr, std::unordered_map<MaterialPtr, DrawBatch>> MaterialBatches;

		std::vector<ObjectData> ObjectDatas;

		std::vector<MultiDrawIndirectCommand> DrawCommands;

		RenderBatch(Ref<FRenderQueue> queue, uint32_t maxObjects);

		void Build(const SubMeshSubmissions &bucket);

		void Draw(FPass &pass, BufferPtr indirect);

		uint32_t InstanceCount() const { return static_cast<uint32_t>(ObjectDatas.size()); }

	private:
		Ref<FRenderQueue> mQueue;
	};
} // namespace BHive