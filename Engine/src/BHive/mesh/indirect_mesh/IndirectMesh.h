#pragma once

#include "core/Core.h"
#include "math/Transform.h"

namespace BHive
{
	class StorageBuffer;
	class BaseMesh;
	struct FSubMesh;

	class IndirectRenderable
	{
	public:
		IndirectRenderable() = default;

		void Init(const Ref<BaseMesh> &renderable, uint32_t instances = 1, bool bones = false);
		void Draw(
			const FTransform &objectMatrix, const glm::mat4 *matrices = nullptr, const glm::mat4 *bones = nullptr,
			size_t bone_count = 0);

		const Ref<BaseMesh> &GetRenderable() const { return mRenderable; }

	private:
		void InitDrawCmdBuffers(const std::vector<FSubMesh> &meshes);
		void AllocatePerObjectBuffer(const std::vector<FSubMesh> &meshes);
		void UpdateObjectData(const FTransform &objectMatrix);

		Ref<StorageBuffer> mDrawBuffer;
		Ref<StorageBuffer> mPerObjectBuffer;
		Ref<StorageBuffer> mBoneBuffer;

		size_t mNumMeshes = 0;
		Ref<BaseMesh> mRenderable;

		Ref<StorageBuffer> mInstanceBuffer;
		uint32_t mInstances = 1;
	};
} // namespace BHive