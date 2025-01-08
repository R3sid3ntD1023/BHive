#pragma once

#include "ColliderComponent.h"


namespace BHive
{
	class StaticMesh;

	class MeshColliderComponent : public ColliderComponent
	{
	public:

		virtual AABB GetBoundingBox() const { return FBox{}; };

		virtual void* GetCollisionShape(const FTransform& world_transform) override;
		virtual void ReleaseCollisionShape() override;
		virtual void OnRender(class SceneRenderer* renderer);

		void SetStaticMesh(const TAssetHandle<StaticMesh>& mesh);
		const TAssetHandle<StaticMesh>& GetStaticMesh() const { return mStaticMesh; }

		
		void Serialize(StreamWriter& ar) const;
		void Deserialize(StreamReader& ar);

		REFLECTABLEV(ColliderComponent)

	private:
		TAssetHandle<StaticMesh> mStaticMesh;
	private:
		void CreateConvexMesh();

		void *mConvexMesh = nullptr;
	};

	REFLECT_EXTERN(MeshColliderComponent)
}  // namespace BHive