#pragma once

#include "core/Core.h"
#include "core/math/Transform.h"
#include "gfx/Buffers.h"

namespace BHive
{
	struct FPerObjectData
	{
		glm::mat4 WorldMatrix = {1.0f};
	};

	struct BHIVE_API FModelBuffer
	{
		void Init(uint32_t maxObjects = 10000);

		uint32_t Submit(const FTransform &transform);

		void Upload();

		void Reset();

		Ref<GPUBuffer> GetObjectBuffer() const { return mObjectBuffer; }

	private:
		Ref<GPUBuffer> mBoneBuffer;
		Ref<GPUBuffer> mObjectBuffer;
		Ref<GPUBuffer> mInstanceBuffer;
		Ref<GPUBuffer> mIndirectBuffer;
		std::vector<FPerObjectData> mBatch;
		uint32_t mMaxObjects = 0;
	};
} // namespace BHive