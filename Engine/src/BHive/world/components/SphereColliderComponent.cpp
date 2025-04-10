#include "GameObject.h"
#include "renderers/Renderer.h"
#include "SphereColliderComponent.h"
#include <physx/PxPhysicsAPI.h>

namespace BHive
{
	void SphereColliderComponent::Render()
	{
		LineRenderer::DrawSphere(Radius, 16, Offset, Color, GetWorldTransform());
	}

	void SphereColliderComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		ColliderComponent::Save(ar);
		ar(Radius);
	}

	void SphereColliderComponent::Load(cereal::BinaryInputArchive &ar)
	{
		ColliderComponent::Load(ar);
		ar(Radius);
	}

	void *SphereColliderComponent::GetGeometry()
	{
		return new physx::PxSphereGeometry(Radius);
	}

	REFLECT(SphereColliderComponent)
	{
		BEGIN_REFLECT(SphereColliderComponent)(META_DATA(ClassMetaData_ComponentSpawnable, true)) REFLECT_CONSTRUCTOR()
			COMPONENT_IMPL() REFLECT_PROPERTY(Radius)(META_DATA(EPropertyMetaData_Default, .5f));
	}
} // namespace BHive