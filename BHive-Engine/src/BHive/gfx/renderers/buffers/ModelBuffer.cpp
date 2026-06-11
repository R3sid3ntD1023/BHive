#include "ModelBuffer.h"
#include "gfx/Buffers.h"
#include "gfx/renderers/Renderer.h"

namespace BHive
{
	#define SSBO_OBJECT_BATCH_BINDING 1
	#define SSBO_INSTANCE_BINDING 2
	#define SSBO_BONE_BINDING 3
	#define MAX_BONES 200
	#define MAX_INSTANCES 10'000

	
	void FModelBuffer::Init(uint32_t maxObjects)
	{
		mMaxObjects = maxObjects;
		mBatch.reserve(maxObjects);

		mObjectBuffer = GPUBuffer::Create(sizeof(FPerObjectData) * maxObjects, EBufferType::StorageBuffer);
	}

	uint32_t FModelBuffer::Submit(const FTransform &transform)
	{
		FPerObjectData data{};
		data.WorldMatrix = transform.ToMat4();
		mBatch.emplace_back(data);
		return (uint32_t)(mBatch.size()) - 1; //gl_DrawID
	}
	
	void FModelBuffer::Upload()
	{
		if (mBatch.empty())
			return;

		mObjectBuffer->SetData(mBatch.data(), mBatch.size() * sizeof(FPerObjectData));
	}

	void FModelBuffer::Reset()
	{
		mBatch.clear();
	}


} // namespace BHive