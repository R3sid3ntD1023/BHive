#include "ModelBuffer.h"
#include "gfx/RenderCommand.h"
#include "gfx/StorageBuffer.h"
#include "renderers/QuadRenderer.h"

namespace BHive
{
#define SSBO_INDEX_PER_OBJECT_BINDING 0
#define SSBO_INSTANCE_BINDING 1
#define SSBO_BONE_BINDING 2
#define MAX_BONES 200
#define MAX_INSTANCES 10'000

	void ModelBuffer::Init()
	{
		BoneBuffer = CreateRef<StorageBuffer>(sizeof(glm::mat4) * MAX_BONES);
		WorldMatrixBuffer = CreateRef<StorageBuffer>(sizeof(glm::mat4));
		InstanceBuffer = CreateRef<StorageBuffer>(sizeof(glm::mat4) * MAX_INSTANCES);
		mIndirectBuffer = CreateRef<StorageBuffer>(sizeof(MultiDrawIndirectCommand));
	}

	void ModelBuffer::Draw(const Ref<FMeshRenderData> &data)
	{
		auto type = data->GetRenderDataType();
		if (type == FMeshRenderData::Type::Skeletal || type == FMeshRenderData::Static)
		{
			DrawMesh(Cast<FStaticMeshRenderData>(data));
		}
	}

	void ModelBuffer::DrawMesh(const Ref<FStaticMeshRenderData> &data)
	{
		if (!data)
			return;

		uint32_t instance_count = (uint32_t)data->InstanceInfo.Transforms.size();

		MultiDrawIndirectCommand command{};
		command.InstanceCount = 1;
		command.BaseInstance = 1;
		command.BaseVertex = data->SubMesh.StartVertex;
		command.FirstIndex = data->SubMesh.StartIndex;
		command.Count = data->SubMesh.IndexCount;

		if (instance_count)
		{
			const auto &instances = data->InstanceInfo.Transforms;

			InstanceBuffer->SetData(instances.data(), sizeof(glm::mat4) * instances.size());
			InstanceBuffer->BindBufferBase(SSBO_INSTANCE_BINDING);
			command.InstanceCount = instance_count;
		}

		if (data->GetRenderDataType() == FMeshRenderData::Skeletal)
		{
			auto skeletaldata = std::static_pointer_cast<FSkeletalMeshRenderData>(data);

			if (!skeletaldata->BoneInfo.Bones.size())
			{
				LOG_WARN("Skeletal mesh render data has no bone info!");
				return;
			}
			const auto &joints = skeletaldata->BoneInfo.Bones;

			BoneBuffer->SetData(joints.data(), joints.size() * sizeof(glm::mat4));
			BoneBuffer->BindBufferBase(SSBO_BONE_BINDING);
		}

		auto matrix = data->Transform.to_mat4() * data->SubMesh.Transformation;
		WorldMatrixBuffer->SetData(&matrix, sizeof(glm::mat4));
		WorldMatrixBuffer->BindBufferBase(SSBO_INDEX_PER_OBJECT_BINDING);

		mIndirectBuffer->SetData(&command, sizeof(MultiDrawIndirectCommand));

		// RenderCommand::DrawElementsBaseVertex(EDrawMode::Triangles, *data->VertexArray, data->SubMesh.StartVertex, data->SubMesh.StartIndex, data->SubMesh.IndexCount, instance_count);
		RenderCommand::MultiDrawElementsIndirect(EDrawMode::Triangles, *mIndirectBuffer, *data->VertexArray, &command, 1, 0);
	}
} // namespace BHive