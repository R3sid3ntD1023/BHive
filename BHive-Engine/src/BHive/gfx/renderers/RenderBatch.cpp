#include "gfx/rendergraph/Commands.h"
#include "gfx/VertexArray.h"
#include "RenderBatch.h"

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
			auto &ctx = mQueue->ResolveContext(o.Context);
			auto &batch = MaterialBatches[ctx.VAO][o.Material][ctx.ShaderMeshType];
			batch.Submissions.push_back(&o);
		}

		for (auto &[vao, materials] : MaterialBatches)
		{
			for (auto &[material, variants] : materials)
			{
				for (auto &[meshType, batch] : variants)
				{
					batch.FirstCommand = static_cast<uint32_t>(DrawCommands.size());
					for (const auto *submission : batch.Submissions)
					{
						auto &ctx = mQueue->ResolveContext(submission->Context);
						const auto &submesh = submission->SubMesh;
						auto model = ctx.Transform;
						auto transform = model * submesh.Transformation;
						glm::vec3 worldCenter = transform.TransformPoint(submesh.Bounds.GetCenter());
						float radius = submesh.Bounds.GetRadius() * glm::compMax(model.Scale);

						auto &instance = ObjectDatas.emplace_back();
						instance.CenterRadius = glm::vec4(worldCenter, radius);
						instance.ModelMatrix = transform.ToMat4();
						instance.ID = static_cast<uint32_t>(ObjectDatas.size() - 1);
						instance.BoneOffset = ctx.BoneOffset;

						auto &command = DrawCommands.emplace_back();
						command.indexCount = submesh.IndexCount;
						command.instanceCount = 0;
						command.firstIndex = ctx.BaseIndex + submesh.StartIndex;
						command.vertexOffset = static_cast<int32_t>(ctx.BaseVertex + submesh.StartVertex);
						command.firstInstance = UINT32_MAX;
					}
					batch.CommandCount = static_cast<uint32_t>(batch.Submissions.size());
				}
			}
		}
	}

	void RenderBatch::Draw(FPass &pass, BufferPtr indirect)
	{
		// render meshes
		for (auto &[vao, matMap] : MaterialBatches)
		{
			auto v = vao.As<VertexArray>();
			v->DeclareAccess(pass);
			pass.Emplace<CmdBindVertexArray>()(vao);

			for (auto &[material, variants] : matMap)
			{
				if (!material)
					continue;

				for (auto &[meshType, batch] : variants)
				{
					pass.Emplace<CmdBindMaterial>()(material.get(), meshType);
					uint32_t offset = batch.FirstCommand * sizeof(MultiDrawIndirectCommand);
					pass.UseBuffer(indirect, EBufferUsage::IndirectRead);
					pass.Emplace<CmdMultiDrawIndexedIndirect>()(ETopologyMode::Triangles, indirect, batch.CommandCount, sizeof(MultiDrawIndirectCommand), offset);
				}
			}
		}
	}
} // namespace BHive