#pragma once

#include "Animator/Animator.h"
#include "core/SubClassOf.h"
#include "mesh/SkeletalMesh.h"
#include "MeshComponent.h"

namespace BHive
{
	class SkeletalMesh;
	class Animator;
	class SkeletalPose;

	struct SkeletalMeshComponent : public MeshComponent
	{
		SkeletalMeshComponent() = default;
		SkeletalMeshComponent(const TAssetHandle<SkeletalMesh> &mesh);

		void SetSkeletalMesh(const TAssetHandle<SkeletalMesh> &mesh);

		const TAssetHandle<SkeletalMesh> &GetSkeletalMesh() const;

		virtual AABB GetBoundingBox() const;

		void OnRender(SceneRenderer *renderer) override;

		TAssetHandle<SkeletalMesh> mSkeletalMesh;

		TSubClassOf<Animator> mAnimatorClass;

		const std::vector<glm::mat4> &GetBoneTransforms() const;

		Ref<SkeletalPose> GetPose() { return mPose; }

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;

		virtual void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLEV(MeshComponent)

	private:
		Ref<Animator> mAnimator;
		Ref<SkeletalPose> mPose;
	};

	REFLECT_EXTERN(SkeletalMeshComponent);
} // namespace BHive