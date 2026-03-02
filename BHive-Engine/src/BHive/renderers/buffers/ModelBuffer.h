#pragma once

#include "core/Core.h"
#include "renderers/RenderData.h"

namespace BHive
{
	class StorageBuffer;

	struct FPerObjectData
	{
		glm::mat4 WorldMatrix = {1.0f};
	};

	struct ModelBuffer
	{
		void Init(uint32_t maxObjects = 10000);

		uint32_t Submit(const FTransform &transform);

		void Upload();

		void Reset();

	private:
		Ref<StorageBuffer> mBoneBuffer{};
		Ref<StorageBuffer> mObjectBuffer{};
		Ref<StorageBuffer> mInstanceBuffer{};
		Ref<StorageBuffer> mIndirectBuffer{};
		std::vector<FPerObjectData> mBatch;
		uint32_t mMaxObjects = 0;
	};
} // namespace BHive