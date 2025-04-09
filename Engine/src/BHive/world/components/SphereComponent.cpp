#include "GameObject.h"
#include "renderers/Renderer.h"
#include "SphereComponent.h"
#include <physx/PxPhysicsAPI.h>

namespace BHive
{
	void SphereComponent::Render()
	{
		LineRenderer::DrawSphere(Radius, 16, Offset, Color, GetWorldTransform());
	}

	void SphereComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		ColliderComponent::Save(ar);
		ar(Radius);
	}

	void SphereComponent::Load(cereal::BinaryInputArchive &ar)
	{
		ColliderComponent::Load(ar);
		ar(Radius);
	}

	void *SphereComponent::GetGeometry()
	{
		return new physx::PxSphereGeometry(Radius);
	}

	REFLECT(SphereComponent)
	{
		BEGIN_REFLECT(SphereComponent)(META_DATA(ClassMetaData_ComponentSpawnable, true)) REFLECT_CONSTRUCTOR()
			COMPONENT_IMPL() REFLECT_PROPERTY(Radius)(META_DATA(EPropertyMetaData_Default, .5f));
	}
} // namespace BHive