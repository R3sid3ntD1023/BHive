#pragma once

#include "ColliderComponent.h"
#include "math/AABB.hpp"
#include "math/Transform.h"

namespace BHive
{
	struct BoxComponent : public ColliderComponent
	{
		BoxComponent() = default;
		BoxComponent(const BoxComponent &) = default;

		glm::vec3 Extents{0.5f};

		virtual void Render() override;
		virtual void Save(cereal::BinaryOutputArchive &ar) const override;
		virtual void Load(cereal::BinaryInputArchive &ar) override;

		virtual AABB GetBoundingBox() const { return FBox{{}, Extents}; }

		void *GetGeometry(const FTransform &transform) override;

		REFLECTABLEV(ColliderComponent)

	private:
		void *SoftBodyShape = nullptr;
	};

	REFLECT_EXTERN(BoxComponent)

} // namespace BHive