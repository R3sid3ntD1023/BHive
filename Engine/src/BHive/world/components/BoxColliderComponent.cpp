#include "BoxColliderComponent.h"
#include "GameObject.h"
#include <physx/PxPhysicsAPI.h>

namespace BHive
{

	void BoxColliderComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		ColliderComponent::Save(ar);
		ar(Extents);
	}

	void BoxColliderComponent::Load(cereal::BinaryInputArchive &ar)
	{
		ColliderComponent::Load(ar);
		ar(Extents);
	}

	void *BoxColliderComponent::GetGeometry()
	{
		auto scale = GetOwner()->GetWorldTransform().GetScale();
		return new physx::PxBoxGeometry(Extents.x * scale.x, Extents.y * scale.y, Extents.z * scale.z);
	}

	REFLECT(BoxColliderComponent)
	{
		BEGIN_REFLECT(BoxColliderComponent)
		(META_DATA(ClassMetaData_ComponentSpawnable, true)) REFLECT_CONSTRUCTOR() REFLECT_PROPERTY(Extents)(META_DATA(EPropertyMetaData_Default, glm::vec3{.5f COMMA.5f COMMA.5f})) COMPONENT_IMPL();
	}

} // namespace BHive