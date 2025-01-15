#pragma once

#include "MeshComponent.h"

namespace BHive
{

	class StaticMesh;

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

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;

		virtual void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLEV(MeshComponent)
	};

	REFLECT_EXTERN(StaticMeshComponent)
} // namespace BHive