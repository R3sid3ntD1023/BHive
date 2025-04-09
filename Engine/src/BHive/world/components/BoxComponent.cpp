#include "BoxComponent.h"
#include "GameObject.h"
#include "physics/PhysicsContext.h"
#include "renderers/Renderer.h"
#include <physx/PxPhysicsAPI.h>

namespace BHive
{
	void BoxComponent::Render()
	{
		auto object = GetOwner();
		auto t = object->GetTransform();
		LineRenderer::DrawBox(Extents, Offset, Color, t);
	}

	void BoxComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		ColliderComponent::Save(ar);
		ar(Extents);
	}

	void BoxComponent::Load(cereal::BinaryInputArchive &ar)
	{
		ColliderComponent::Load(ar);
		ar(Extents);
	}

	void *BoxComponent::GetGeometry(const FTransform &transform)
	{
		auto extents = Extents * transform.get_scale();
		return new physx::PxBoxGeometry(extents.x, extents.y, extents.z);
	}

	REFLECT(BoxComponent)
	{
		BEGIN_REFLECT(BoxComponent)
		(META_DATA(ClassMetaData_ComponentSpawnable, true)) REFLECT_CONSTRUCTOR() REFLECT_PROPERTY(Extents) COMPONENT_IMPL();
	}

} // namespace BHive