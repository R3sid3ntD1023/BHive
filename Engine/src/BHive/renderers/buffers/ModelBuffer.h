#pragma once

#include "core/Core.h"
#include "renderers/RenderData.h"

namespace BHive
{
	class StorageBuffer;

	struct ModelBuffer
	{
		void Init();

		void Submit(const Ref<FMeshRenderData> &data);

	private:
		Ref<StorageBuffer> BoneBuffer;
		Ref<StorageBuffer> WorldMatrixBuffer;
		Ref<StorageBuffer> InstanceBuffer;
	};
} // namespace BHive