#pragma once

#include "RenderQueue.h"
#include "gfx/rendergraph/Pass.h"

namespace BHive
{

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