#pragma once

#include "ColliderComponent.h"

namespace BHive
{
	struct BoxColliderComponent : public ColliderComponent
	{

		glm::vec3 Extents{0.5f};

		virtual void Render() override;
		virtual void Save(cereal::BinaryOutputArchive &ar) const override;
		virtual void Load(cereal::BinaryInputArchive &ar) override;

		virtual AABB GetBoundingBox() const { return FBox{{}, Extents}; }

		void *GetGeometry() override;

		REFLECTABLEV(ColliderComponent)
	};

	REFLECT_EXTERN(BoxColliderComponent)

} // namespace BHive