#pragma once

#include "IResourceFactory.h"
#include "gfx/animation/SkeletalAnimation.h"
#include "gfx/mesh/BaseMesh.h"
#include "gfx/mesh/SkeletalMesh.h"
#include "gfx/mesh/StaticMesh.h"
#include "importers/MeshImporter.h"

namespace BHive
{
	class StaticMesh;
	class SkeletalMesh;

	struct BHIVE_API MeshFactory
	{
		static Ref<StaticMesh> CreateStatic(const FMeshData &meshData);

		static Ref<SkeletalMesh> CreateSkeletal(const FMeshData &meshData, SkeletonPtr skeleton);

		static Ref<StaticMesh> CreateCube(float size = 1.0f);

		static Ref<StaticMesh> CreateSphere(float radius, uint32_t sectors = 32u, uint32_t stacks = 32u);

		static Ref<StaticMesh> CreatePlane(float x = 1.0f, float y = 1.0f);
	};

	struct BHIVE_API SkeletonFactory : public IResourceFactory<Skeleton>
	{
		static SkeletonPtr Create(const BoneInfo &boneInfo, const SkeletalNode &root);
	};

	struct BHIVE_API SkeletalAnimationFactory : public IResourceFactory<SkeletalAnimation>
	{
		static SkeletalAnimationPtr Create(const DecodedAnimation &decoded);

		static SkeletalAnimationPtr Create(float duration, float ticksPerSecond, const SkeletalAnimation::Frames &frames);
	};

} // namespace BHive