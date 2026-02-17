#pragma once

#include "core/Core.h"
#include "renderers/RenderData.h"

namespace BHive
{
	class StorageBuffer;
	class VertexBuffer;
	class Shader;

	struct ModelBuffer
	{
		void Init();

		void Draw(const Ref<FMeshRenderData> &data);

		void DrawMesh(const Ref<FStaticMeshRenderData> &data);

		void SubmitModel(const FTransform &transform);

	private:
		Ref<StorageBuffer> mBoneBuffer{};
		Ref<StorageBuffer> mPerObjectBuffer{};
		Ref<StorageBuffer> mInstanceBuffer{};
		Ref<StorageBuffer> mIndirectBuffer{};
		Ref<Shader> mComputeInstanceShader{};
	};
} // namespace BHive