#pragma once

#include "ColliderComponent.h"
#include "math/AABB.hpp"
#include "math/Transform.h"

namespace BHive
{
	DECLARE_CLASS(BHive::ClassMetaData_ComponentSpawnable)
	struct BoxComponent : public ColliderComponent
	{
		DECLARE_CONSTRUCTOR()
		BoxComponent() = default;
		BoxComponent(const BoxComponent &) = default;

		DECLARE_PROPERTY()
		glm::vec3 Extents{0.5f};

		virtual void Render() override;
		virtual void Save(cereal::BinaryOutputArchive &ar) const override;
		virtual void Load(cereal::BinaryInputArchive &ar) override;

		virtual AABB GetBoundingBox() const { return FBox{{}, Extents}; }

		void *GetCollisionShape(const FTransform &world_transform) override;
		void OnReleaseCollisionShape() override;

		REFLECTABLEV(ColliderComponent)
	};

} // namespace BHive