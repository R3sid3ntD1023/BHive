#include "gfx/RenderCommand.h"
#include "gfx/Shader.h"
#include "gfx/ShaderManager.h"
#include "gfx/StorageBuffer.h"
#include "ModelBuffer.h"
#include "GlobalBuffers.h"

namespace BHive
{
#define SSBO_INDEX_PER_OBJECT_BINDING 1
#define SSBO_INSTANCE_BINDING 2
#define SSBO_BONE_BINDING 3
#define MAX_BONES 200
#define MAX_INSTANCES 10'000

	struct FPerObjectData
	{
		glm::mat4 WorldMatrix = {1.0f};
		// uint32_t InstanceCount = 0;
	};

	void ModelBuffer::Init()
	{
		//mBoneBuffer = StorageBuffer::Create(sizeof(glm::mat4) * MAX_BONES);
		mPerObjectBuffer = StorageBuffer::Create(sizeof(FPerObjectData));
		/*mInstanceBuffer = StorageBuffer::Create(sizeof(glm::mat4) * MAX_INSTANCES);
		mIndirectBuffer = StorageBuffer::Create(sizeof(MultiDrawIndirectCommand));

		mComputeInstanceShader = ShaderManager::Get().Load("ComputeInstances.glsl");*/

		GlobalBuffers::AddGlobalStorageBuffer(SSBO_INDEX_PER_OBJECT_BINDING, mPerObjectBuffer);
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

		//uint32_t instance_count = (uint32_t)data->Instances.Transforms.size();

		//if (instance_count)
		//{
		//	const auto &instances = data->Instances.Transforms;

		//	mInstanceBuffer->SetData(instances.data(), sizeof(glm::mat4) * instances.size());
		//	// mInstanceBuffer->BindBufferBase(SSBO_INSTANCE_BINDING);
		//	mComputeInstanceShader->Dispatch(instance_count, 1, 1);
		//}

		//if (data->GetRenderDataType() == FMeshRenderData::Skeletal)
		//{
		//	auto skeletaldata = std::static_pointer_cast<FSkeletalMeshRenderData>(data);

		//	if (!skeletaldata->Bones.Bones.size())
		//	{
		//		LOG_WARN("Skeletal mesh render data has no bone info!");
		//		return;
		//	}
		//	const auto &joints = skeletaldata->Bones.Bones;

		//	mBoneBuffer->SetData(joints.data(), joints.size() * sizeof(glm::mat4));
		//	// mBoneBuffer->BindBufferBase(SSBO_BONE_BINDING);
		//}

		FPerObjectData object_data{};
		object_data.WorldMatrix = data->Transform.to_mat4() * data->SubMesh.Transformation;

		mPerObjectBuffer->SetData(&object_data, sizeof(FPerObjectData));
		// mPerObjectBuffer->BindBufferBase(SSBO_INDEX_PER_OBJECT_BINDING);

		/*MultiDrawIndirectCommand command{};
		command.InstanceCount = instance_count ? instance_count : 1;
		command.BaseInstance = 0;
		command.BaseVertex = data->SubMesh.StartVertex;
		command.FirstIndex = data->SubMesh.StartIndex;
		command.Count = data->SubMesh.IndexCount;

		mIndirectBuffer->SetData(&command, sizeof(MultiDrawIndirectCommand));

		RenderCommand::MultiDrawElementsIndirect(ETopologyMode::Triangles, *mIndirectBuffer, *data->VertexArray, &command, 1, 0);*/
	}

	void ModelBuffer::SubmitModel(const FTransform &transform)
	{
		FPerObjectData object_data{};
		object_data.WorldMatrix = transform;

		mPerObjectBuffer->SetData(&object_data, sizeof(FPerObjectData));
	}
} // namespace BHive