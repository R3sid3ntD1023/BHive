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

		void Serialize(StreamWriter &ar) const;
		void Deserialize(StreamReader &ar);

		REFLECTABLEV(MeshComponent)

	private:
		Ref<SkeletalPose> mPose;
	};

	REFLECT_EXTERN(SkeletalMeshComponent);
}