#pragma once

#include "World/Component.h"

namespace BHive
{
	class SkeletalMesh;

	class SkeletalMeshComponent : public Component
	{
	public:
		SkeletalMeshComponent() = default;

		void Render() override;

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;

		virtual void Load(cereal::BinaryInputArchive &ar) override;

		Ref<SkeletalMesh> SkeletalMeshAsset;

		REFLECTABLEV(Component)
	};

	REFLECT_EXTERN(SkeletalMeshComponent)
} // namespace BHive