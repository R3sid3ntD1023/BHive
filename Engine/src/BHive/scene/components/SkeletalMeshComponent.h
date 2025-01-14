#pragma once

#include "MeshComponent.h"
#include "mesh/SkeletalMesh.h"

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

		const TAssetHandle<SkeletalMesh> &GetSkeletalMesh() const { return mSkeletalMesh; }

		virtual AABB GetBoundingBox() const;

		void OnRender(SceneRenderer *renderer) override;

		TAssetHandle<SkeletalMesh> mSkeletalMesh;

		Ref<Animator> mAnimator;

		const std::vector<glm::mat4> &GetBoneTransforms() const;

		Ref<SkeletalPose> GetPose() { return mPose; }

		virtual void Save(cereal::JSONOutputArchive &ar) const override;

		virtual void Load(cereal::JSONInputArchive &ar) override;

		REFLECTABLEV(MeshComponent)

	private:
		Ref<SkeletalPose> mPose;
	};

	REFLECT_EXTERN(SkeletalMeshComponent);
} // namespace BHive