#include "ModelBuffer.h"
#include "gfx/RenderCommand.h"
#include "gfx/StorageBuffer.h"

namespace BHive
{
#define SSBO_INDEX_PER_OBJECT_BINDING 1
#define SSBO_INSTANCE_BINDING 2
#define SSBO_BONE_BINDING 3
#define MAX_BONES 200
#define MAX_INSTANCES 1000

	void ModelBuffer::Init()
	{
		BoneBuffer = CreateRef<StorageBuffer>(sizeof(glm::mat4) * MAX_BONES);
		WorldMatrixBuffer = CreateRef<StorageBuffer>(sizeof(glm::mat4));
		InstanceBuffer = CreateRef<StorageBuffer>(sizeof(glm::mat4) * MAX_INSTANCES);
	}

	void ModelBuffer::Submit(const Ref<FMeshRenderData> &data)
	{
		auto matrix = data->Transform * data->SubMesh.Transformation;
		WorldMatrixBuffer->SetData(&matrix, sizeof(glm::mat4));
		WorldMatrixBuffer->BindBufferBase(SSBO_INDEX_PER_OBJECT_BINDING);

		if (data->InstanceCount > 0)
		{
			InstanceBuffer->SetData(data->Instances, sizeof(glm::mat4) * data->InstanceCount);
			InstanceBuffer->BindBufferBase(SSBO_INSTANCE_BINDING);
		}

		if (const auto skeletaldata = Cast<FSkeletalMeshRenderData>(data))
		{
			const auto &joints = skeletaldata->Bones;
			BoneBuffer->SetData(joints.data(), joints.size() * sizeof(glm::mat4));
			BoneBuffer->BindBufferBase(SSBO_BONE_BINDING);
		}

		RenderCommand::DrawElementsBaseVertex(EDrawMode::Triangles, *data->VertexArray, data->SubMesh.StartVertex, data->SubMesh.StartIndex, data->SubMesh.IndexCount, data->InstanceCount);
	}
} // namespace BHive