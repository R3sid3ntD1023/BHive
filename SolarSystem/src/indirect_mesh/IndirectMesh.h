#pragma once

#include "core/Core.h"
#include "math/Transform.h"

namespace BHive
{
	class StorageBuffer;
	class IRenderableAsset;
	struct FSubMesh;

	class IndirectRenderable
	{
	public:
		IndirectRenderable() = default;

		void Init(const Ref<IRenderableAsset> &renderable);
		void Init(const Ref<IRenderableAsset> &renderable, uint32_t instances);
		void Draw(const FTransform &objectMatrix);
		void Draw(const FTransform &objectMatrix, const glm::mat4 *matrices);

		const Ref<IRenderableAsset> &GetRenderable() const { return mRenderable; }

	private:
		void InitDrawCmdBuffers(const std::vector<FSubMesh> &meshes);
		void AllocatePerObjectBuffer(const std::vector<FSubMesh> &meshes);
		void UpdateObjectData(const FTransform &objectMatrix);

		Ref<StorageBuffer> mDrawBuffer;
		Ref<StorageBuffer> mPerObjectBuffer;

		size_t mNumMeshes = 0;
		Ref<IRenderableAsset> mRenderable;

		Ref<StorageBuffer> mInstanceBuffer;
		uint32_t mInstances = 1;
	};
} // namespace BHive