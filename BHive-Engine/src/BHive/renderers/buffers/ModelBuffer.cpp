#include "gfx/RenderCommand.h"
#include "gfx/Shader.h"
#include "gfx/ShaderManager.h"
#include "gfx/StorageBuffer.h"
#include "ModelBuffer.h"
#include "gfx/GlobalBuffers.h"
#include "core/subsystem/SubSystem.h"

namespace BHive
{
	#define SSBO_OBJECT_BATCH_BINDING 1
	#define SSBO_INSTANCE_BINDING 2
	#define SSBO_BONE_BINDING 3
	#define MAX_BONES 200
	#define MAX_INSTANCES 10'000

	
	void ModelBuffer::Init(uint32_t maxObjects)
	{
		mMaxObjects = maxObjects;
		mBatch.reserve(maxObjects);

		mObjectBuffer = StorageBuffer::Create(sizeof(FPerObjectData) * maxObjects);
		GetSubSystem<GlobalBuffers>().Register(SSBO_OBJECT_BATCH_BINDING, {.Buffer  = mObjectBuffer});
	}

	uint32_t ModelBuffer::Submit(const FTransform &transform)
	{
		FPerObjectData data{};
		data.WorldMatrix = transform.ToMat4();
		mBatch.emplace_back(data);
		return (uint32_t)(mBatch.size()) - 1; //gl_DrawID
	}
	
	void ModelBuffer::Upload()
	{
		if (mBatch.empty())
			return;

		mObjectBuffer->SetData(mBatch.data(), mBatch.size() * sizeof(FPerObjectData));
	}

	void ModelBuffer::Reset()
	{
		mBatch.clear();
	}

} // namespace BHive