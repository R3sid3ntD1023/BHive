#include "ModelBuffer.h"
#include "gfx/RenderCommand.h"
#include "gfx/StorageBuffer.h"
#include "renderers/QuadRenderer.h"

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
		uint32_t instance_count = 0;

		if (data->InstanceInfo)
		{
			const auto &instances = data->InstanceInfo->Transforms;

			InstanceBuffer->SetData(instances.data(), sizeof(glm::mat4) * instances.size());
			InstanceBuffer->BindBufferBase(SSBO_INSTANCE_BINDING);
			instance_count = static_cast<uint32_t>(instances.size());
		}

		if (data->GetRenderDataType() == FMeshRenderData::Skeletal)
		{
			auto skeletaldata = std::static_pointer_cast<FSkeletalMeshRenderData>(data);

			if (!skeletaldata->BoneInfo)
			{
				LOG_WARN("Skeletal mesh render data has no bone info!");
				return;
			}
			const auto &joints = skeletaldata->BoneInfo->Bones;
			BoneBuffer->SetData(joints.data(), joints.size() * sizeof(glm::mat4));
			BoneBuffer->BindBufferBase(SSBO_BONE_BINDING);
		}

		auto matrix = data->ObjectInfo.Transform.to_mat4() * data->SubMesh.Transformation;
		WorldMatrixBuffer->SetData(&matrix, sizeof(glm::mat4));
		WorldMatrixBuffer->BindBufferBase(SSBO_INDEX_PER_OBJECT_BINDING);

		RenderCommand::DrawElementsBaseVertex(EDrawMode::Triangles, *data->VertexArray, data->SubMesh.StartVertex, data->SubMesh.StartIndex, data->SubMesh.IndexCount, instance_count);
	}
} // namespace BHive