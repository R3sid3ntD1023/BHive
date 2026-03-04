#pragma once

#include "core/Core.h"
#include "core/math/Transform.h"
#include "gfx/StorageBuffer.h"

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

		Ref<StorageBuffer> GetObjectBuffer() const { return mObjectBuffer; }

	private:
		Ref<StorageBuffer> mBoneBuffer;
		Ref<StorageBuffer> mObjectBuffer;
		Ref<StorageBuffer> mInstanceBuffer;
		Ref<StorageBuffer> mIndirectBuffer;
		std::vector<FPerObjectData> mBatch;
		uint32_t mMaxObjects = 0;
	};
} // namespace BHive