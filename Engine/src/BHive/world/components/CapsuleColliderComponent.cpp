#include "CapsuleColliderComponent.h"
#include "physics/PhysicsCore.h"
#include "renderers/LineRenderer.h"
#include <physx/PxPhysicsAPI.h>
#include "GameObject.h"

namespace BHive
{
	void CapsuleColliderComponent::Render()
	{
		LineRenderer::DrawCapsule(Radius, HalfHeight, 16, Offset, Color, GetWorldTransform());
	}

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
		return new physx::PxCapsuleGeometry(Radius, HalfHeight);
	}

	REFLECT(CapsuleColliderComponent)
	{
		BEGIN_REFLECT(CapsuleColliderComponent)(META_DATA(ClassMetaData_ComponentSpawnable, true)) REFLECT_CONSTRUCTOR()
			REFLECT_PROPERTY(Radius)(META_DATA(EPropertyMetaData_Default, .5f))
				REFLECT_PROPERTY(HalfHeight)(META_DATA(EPropertyMetaData_Default, 1.0f)) COMPONENT_IMPL();
	}
} // namespace BHive