#include "CapsuleColliderComponent.h"
#include <physx/PxPhysicsAPI.h>
#include "runtime/GameObject.h"

namespace BHive
{
	void CapsuleColliderComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		ColliderComponent::Save(ar);
		ar(Radius, HalfHeight);
	}
	void CapsuleColliderComponent::Load(cereal::BinaryInputArchive &ar)
	{
		ColliderComponent::Load(ar);
		ar(Radius, HalfHeight);
	}
	void *CapsuleColliderComponent::GetGeometry()
	{
		auto scale = GetOwner()->GetWorldTransform().GetScale();
		return new physx::PxCapsuleGeometry(Radius * glm::max(scale.x, scale.z), HalfHeight * scale.y);
	}

	REFLECT(CapsuleColliderComponent)
	{
		BEGIN_REFLECT(CapsuleColliderComponent)(META_DATA(ClassMetaData_ComponentSpawnable, true)) REFLECT_CONSTRUCTOR() REFLECT_PROPERTY(Radius)(META_DATA(EPropertyMetaData_Default, .5f))
			REFLECT_PROPERTY(HalfHeight)(META_DATA(EPropertyMetaData_Default, 1.0f)) COMPONENT_IMPL();
	}
} // namespace BHive