#include "RenderBatch.h"
#include "gfx/VertexArray.h"
#include "gfx/rendergraph/Commands.h"

namespace BHive
{
	RenderBatch::RenderBatch(Ref<FRenderQueue> queue, uint32_t maxObjects)
		: mQueue(queue)
	{
		ObjectDatas.reserve(maxObjects);
		DrawCommands.reserve(maxObjects);
	}

	void RenderBatch::Build(const SubMeshSubmissions &bucket)
	{
		MaterialBatches.clear();
		ObjectDatas.clear();
		DrawCommands.clear();

		for (auto &o : bucket)
		{
			// object data
			auto &ctx = mQueue->ResolveContext(o.Context);
			auto model = ctx.Transform;

			auto vao = ctx.VAO;
			auto &s = o.SubMesh;

			auto objectID = ObjectDatas.size();

			auto m = model * s.Transformation;
			glm::vec3 localCenter = s.Bounds.GetCenter();
			glm::vec3 worldCenter = m.TransformPoint(localCenter);
			float radius = s.Bounds.GetRadius() * glm::compMax(model.Scale);

			auto &inst = ObjectDatas.emplace_back();
			inst.CenterRadius = glm::vec4(worldCenter, radius);
			inst.ModelMatrix = m.ToMat4();
			inst.ID = objectID;
			inst.BoneOffset = ctx.BoneOffset;

			// submesh data
			auto &group = MaterialBatches[vao];
			auto &batch = group[o.Material];

			if (batch.CommandCount == 0)
				batch.FirstCommand = (uint32_t)DrawCommands.size();

			batch.CommandCount++;

			auto &cmd = DrawCommands.emplace_back();
			cmd.indexCount = s.IndexCount;
			cmd.instanceCount = 0; // GPU increments this
			cmd.firstIndex = s.StartIndex;
			cmd.vertexOffset = s.StartVertex;
			cmd.firstInstance = UINT32_MAX; // GPU will use visibleIndices[]
		}
	}

	void RenderBatch::Draw(FPass &pass, BufferPtr indirect)
	{
		// render meshes
		for (auto &[vao, matMap] : MaterialBatches)
		{
			auto v = vao.As<VertexArray>();
			v->DeclareAccess(pass, EBufferUsage::IndirectRead, EBufferUsage::IndirectRead);

			for (auto &[material, batch] : matMap)
			{
				if (!material)
					continue;

				pass.Emplace<CmdBindMaterial>()(material.As<Material>());

				uint32_t offset = batch.FirstCommand * sizeof(MultiDrawIndirectCommand);

				pass.Emplace<CmdMultiDrawIndexedIndirect>()(ETopologyMode::Triangles, indirect, vao, batch.CommandCount, sizeof(MultiDrawIndirectCommand), offset);
			}
		}
	}
} // namespace BHive