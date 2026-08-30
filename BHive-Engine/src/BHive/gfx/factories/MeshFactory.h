#pragma once

#include "IResourceFactory.h"
#include "gfx/mesh/BaseMesh.h"
#include "gfx/mesh/SkeletalMesh.h"
#include "gfx/animation/SkeletalAnimation.h"

namespace BHive
{
	struct BHIVE_API MeshFactory : public IResourceFactory<BaseMesh>
	{
		static MeshPtr CreateStatic(const FMeshData &meshData);

		static MeshPtr CreateSkeletal(const FMeshData &meshData, SkeletonPtr skeleton);

		static MeshPtr CreateCube(float size = 1.0f);

		static MeshPtr CreateSphere(float radius, uint32_t sectors = 32u, uint32_t stacks = 32u);

		static MeshPtr CreatePlane(float x = 1.0f, float y = 1.0f);
	};

	struct BHIVE_API SkeletonFactory : public IResourceFactory<Skeleton>
	{
		static SkeletonPtr Create(const Bones &bones, const SkeletalNode &root);
	};

	struct BHIVE_API SkeletalAnimationFactory : public IResourceFactory<SkeletalAnimation>
	{
		static SkeletalAnimationPtr Create(float duration, float ticksPerSecond, const Frames &frames, const glm::mat4 &globalInverseMatrix);
	};

	// class VertexBuffer;
	// class IndexBuffer;
	// class VertexArray;

	// struct MeshManager
	// {
	// 	void Init();

	// 	void Add(MeshHandle h);

	// 	Ref<VertexArray> GetVAO() const { return mVAO; }

	// private:
	// 	uint32_t mCurrentVertexIndex = 0;
	// 	uint32_t mCurrentIndexIndex = 0;

	// 	Ref<VertexBuffer> mVertexBuffer;
	// 	Ref<IndexBuffer> mIndexBuffer;
	// 	Ref<VertexArray> mVAO;
	// };
} // namespace BHive