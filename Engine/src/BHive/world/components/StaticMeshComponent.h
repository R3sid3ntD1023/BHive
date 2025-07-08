#pragma once

#include "Component.h"
#include "mesh/StaticMesh.h"

namespace BHive
{
	struct StaticMeshComponent : public Component
	{
		Ref<StaticMesh> StaticMeshAsset;

		StaticMeshComponent() = default;
		StaticMeshComponent(const StaticMeshComponent &other) = default;

		void Render() override;

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;

		virtual void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLE_CLASS(Component)
	};

	REFLECT_EXTERN(StaticMeshComponent)
} // namespace BHive