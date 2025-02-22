#include "IndirectMesh.h"
#include "mesh/IRenderableAsset.h"
#include "gfx/StorageBuffer.h"
#include "gfx/VertexArray.h"
#include <glad/glad.h>

namespace BHive
{
#define SSBO_INSTANCE_DATA 1
#define SSBO_INDEX_PER_OBJECT_DATA 2

	struct FPerObjectData
	{
		glm::mat4 WorldMatrix;
	};

	struct FDrawElementsIndirectCommand
	{
		uint32_t count;
		uint32_t instanceCount;
		uint32_t firstIndex;
		int32_t baseVertex;
		uint32_t baseInstance;
	};

	void IndirectRenderable::Init(const Ref<IRenderableAsset> &renderable)
	{
		mRenderable = renderable;

		const auto &meshes = renderable->GetSubMeshes();
		mNumMeshes = meshes.size();

		InitDrawCmdBuffers(meshes);
		AllocatePerObjectBuffer(meshes);
	}

	void IndirectRenderable::Init(const Ref<IRenderableAsset> &renderable, uint32_t instances)
	{
		mInstances = instances;
		mInstanceBuffer = StorageBuffer::Create(sizeof(glm::mat4) * mInstances);
		Init(renderable);
	}

	void IndirectRenderable::InitDrawCmdBuffers(const std::vector<FSubMesh> &meshes)
	{
		std::vector<FDrawElementsIndirectCommand> drawCommands;
		drawCommands.resize(meshes.size());

		for (size_t i = 0; i < meshes.size(); i++)
		{
			const auto &mesh = meshes[i];
			auto &drawCommand = drawCommands[i];
			drawCommand.count = mesh.mIndexCount;
			drawCommand.instanceCount = mInstances;
			drawCommand.firstIndex = mesh.mStartIndex;
			drawCommand.baseVertex = mesh.mStartVertex;
			drawCommand.baseInstance = 0;
		}

		mDrawBuffer = StorageBuffer::Create(drawCommands.data(), drawCommands.size() * sizeof(FDrawElementsIndirectCommand));
	}

	void IndirectRenderable::AllocatePerObjectBuffer(const std::vector<FSubMesh> &meshes)
	{
		mPerObjectBuffer = StorageBuffer::Create(meshes.size() * sizeof(FPerObjectData));
	}

	void IndirectRenderable::UpdateObjectData(const FTransform &objectMatrix)
	{
		std::vector<FPerObjectData> perObjectData;
		perObjectData.resize(mNumMeshes);

		for (size_t i = 0; i < mNumMeshes; i++)
		{
			perObjectData[i].WorldMatrix = objectMatrix;
		}

		mPerObjectBuffer->SetData(perObjectData.data(), sizeof(FPerObjectData) * perObjectData.size());
		mPerObjectBuffer->BindBufferBase(SSBO_INDEX_PER_OBJECT_DATA);
	}

	void IndirectRenderable::Draw(const FTransform &objectMatrix)
	{

		mRenderable->GetVertexArray()->Bind();

		UpdateObjectData(objectMatrix);

		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, mDrawBuffer->GetRendererID());

		glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, NULL, mNumMeshes, 0);

		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

		mRenderable->GetVertexArray()->UnBind();
	}

	void IndirectRenderable::Draw(const FTransform &objectMatrix, const glm::mat4 *matrices)
	{
		mInstanceBuffer->SetData(matrices, sizeof(glm::mat4) * mInstances);
		mInstanceBuffer->BindBufferBase(SSBO_INSTANCE_DATA);

		Draw(objectMatrix);
	}

} // namespace BHive