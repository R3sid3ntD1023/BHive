#pragma once

#include "core/Core.h"
#include "renderers/RenderData.h"

namespace BHive
{
	class StorageBuffer;

	struct ModelBuffer
	{
		void Init();

		void Draw(const Ref<FMeshRenderData> &data);

		void DrawMesh(const Ref<FStaticMeshRenderData> &data);

	private:
		Ref<StorageBuffer> BoneBuffer;
		Ref<StorageBuffer> WorldMatrixBuffer;
		Ref<StorageBuffer> InstanceBuffer;
		Ref<StorageBuffer> mIndirectBuffer;
	};
} // namespace BHive