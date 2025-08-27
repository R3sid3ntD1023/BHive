#pragma once

#include "World/Component.h"
#include "mesh/SkeletalMesh.h"

namespace BHive
{
	class SkeletalMesh;

	class SkeletalMeshComponent : public Component
	{
	public:
		SkeletalMeshComponent() = default;

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;

		virtual void Load(cereal::BinaryInputArchive &ar) override;

		void SetSkeletalMesh(const Ref<SkeletalMesh> &mesh);

		const Ref<SkeletalMesh> &GetSkeletalMesh() const { return SkeletalMeshAsset; }

		const MaterialTable &GetMaterials() const { return mOverrideMaterials; }

		REFLECTABLEV(Component)

	private:
		Ref<SkeletalMesh> SkeletalMeshAsset;

		MaterialTable mOverrideMaterials;
	};

	REFLECT_EXTERN(SkeletalMeshComponent)
} // namespace BHive