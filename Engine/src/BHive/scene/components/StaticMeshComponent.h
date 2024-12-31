#pragma once

#include "ShapeComponent.h"
#include "material/MaterialTable.h"
#include "mesh/StaticMesh.h"
#include "mesh/SkeletalMesh.h"

namespace BHive
{

	class AnimationClip;
	class Animator;
	class SkeletalPose;

	struct BHIVE MeshComponent : public ShapeComponent
	{
		
		void Serialize(StreamWriter& ar) const;
		void Deserialize(StreamReader& ar);

		REFLECTABLEV(ShapeComponent)

	protected:
		void SetMaterialTable(const MaterialTable& materials);

	protected:
		MaterialTable mOverrideMaterials;
		MaterialTable mMaterials;
	};

	REFLECT_EXTERN(MeshComponent)

	struct BHIVE StaticMeshComponent : public MeshComponent
	{
		StaticMeshComponent() = default;
		StaticMeshComponent(const TAssetHandle<StaticMesh> &model);

		virtual ~StaticMeshComponent() = default;

		void SetStaticMesh(const TAssetHandle<StaticMesh> &handle);

		const TAssetHandle<StaticMesh> &GetStaticMesh() const { return mStaticMesh; }

		virtual AABB GetBoundingBox() const;

		TAssetHandle<StaticMesh> mStaticMesh;

		void OnRender(SceneRenderer *renderer) override;

		void Serialize(StreamWriter& ar) const;
		void Deserialize(StreamReader& ar);

		REFLECTABLEV(MeshComponent)
	};

	REFLECT_EXTERN(StaticMeshComponent)

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

		void Serialize(StreamWriter& ar) const;
		void Deserialize(StreamReader& ar);

		REFLECTABLEV(MeshComponent)

	private:
		Ref<SkeletalPose> mPose;
	};

	REFLECT_EXTERN(SkeletalMeshComponent);

	struct BHIVE InstancedStaticMeshComponent : public StaticMeshComponent
	{
		InstancedStaticMeshComponent() = default;
		InstancedStaticMeshComponent(const TAssetHandle<StaticMesh> &model)
			: StaticMeshComponent(model)
		{
		}

		std::vector<FTransform> mTransforms;

		REFLECTABLEV(StaticMeshComponent)
	};

}