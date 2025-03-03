#include "IndirectMesh.h"
#include "mesh/BaseMesh.h"
#include "gfx/StorageBuffer.h"
#include "gfx/VertexArray.h"
#include "gfx/RenderCommand.h"
#include <glad/glad.h>

#define SSBO_INDEX_PER_OBJECT_BINDING 1
#define SSBO_INSTANCE_BINDING 2
#define SSBO_BONE_BINDING 3
#define MAX_BONES 128

namespace BHive
{

	struct FPerObjectData
	{
		glm::mat4 WorldMatrix{1.0f};
	};

	struct FDrawElementsIndirectCommand
	{
		uint32_t count;
		uint32_t instanceCount;
		uint32_t firstIndex;
		int32_t baseVertex;
		uint32_t baseInstance;
	};

	void IndirectRenderable::Init(const Ref<BaseMesh> &renderable, uint32_t instances, bool bones)
	{
		mRenderable = renderable;
		mInstances = instances;

		if (mInstances > 1)
		{
			mInstanceBuffer = StorageBuffer::Create(sizeof(glm::mat4) * mInstances);
		}

		if (bones)
		{
			mBoneBuffer = StorageBuffer::Create(sizeof(glm::mat4) * MAX_BONES);
		}

		const auto &meshes = renderable->GetSubMeshes();
		mNumMeshes = meshes.size();

		InitDrawCmdBuffers(meshes);
		AllocatePerObjectBuffer(meshes);
	}

	void IndirectRenderable::Draw(
		const FTransform &objectMatrix, const glm::mat4 *matrices, const glm::mat4 *bones, size_t bone_count)
	{
		if (matrices != nullptr)
		{

			mInstanceBuffer->SetData(matrices, sizeof(glm::mat4) * mInstances);
			mInstanceBuffer->BindBufferBase(SSBO_INSTANCE_BINDING);
		}

		if (bones != nullptr)
		{

			mBoneBuffer->SetData(bones, sizeof(glm::mat4) * bone_count);
			mBoneBuffer->BindBufferBase(SSBO_BONE_BINDING);
		}

		UpdateObjectData(objectMatrix);

		RenderCommand::MultiDrawElementsIndirect(
			mDrawBuffer->GetRendererID(), Triangles, *mRenderable->GetVertexArray(), mNumMeshes, 0);
	}

	void IndirectRenderable::InitDrawCmdBuffers(const std::vector<FSubMesh> &meshes)
	{
		std::vector<FDrawElementsIndirectCommand> drawCommands;
		drawCommands.resize(meshes.size());

		for (size_t i = 0; i < meshes.size(); i++)
		{
			const auto &mesh = meshes[i];
			auto &drawCommand = drawCommands[i];
			drawCommand.count = mesh.IndexCount;
			drawCommand.instanceCount = mInstances;
			drawCommand.firstIndex = mesh.StartIndex;
			drawCommand.baseVertex = mesh.StartVertex;
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

		auto &meshes = mRenderable->GetSubMeshes();
		for (size_t i = 0; i < mNumMeshes; i++)
		{
			glm::mat4 final_transformation = objectMatrix.to_mat4() * meshes[i].Transformation;
			perObjectData[i].WorldMatrix = final_transformation;
		}

		mPerObjectBuffer->BindBufferBase(SSBO_INDEX_PER_OBJECT_BINDING);
		mPerObjectBuffer->SetData(perObjectData.data(), sizeof(FPerObjectData) * perObjectData.size());

#ifdef USE_VERTEX_PULLING
		mRenderable->GetVertexArray()->BindBuffersBase(0);
#endif
	}

} // namespace BHive