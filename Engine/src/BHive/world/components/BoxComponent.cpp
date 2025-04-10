#include "BoxComponent.h"
#include "GameObject.h"
#include "renderers/Renderer.h"
#include <physx/PxPhysicsAPI.h>

namespace BHive
{
	void BoxComponent::Render()
	{
		LineRenderer::DrawBox(Extents, Offset, Color, GetWorldTransform());
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

	void *BoxComponent::GetGeometry()
	{
		return new physx::PxBoxGeometry(Extents.x, Extents.y, Extents.z);
	}

	REFLECT(BoxComponent)
	{
		BEGIN_REFLECT(BoxComponent)
		(META_DATA(ClassMetaData_ComponentSpawnable, true)) REFLECT_CONSTRUCTOR()
			REFLECT_PROPERTY(Extents)(META_DATA(EPropertyMetaData_Default, glm::vec3{.5f COMMA.5f COMMA.5f}))
				COMPONENT_IMPL();
	}

} // namespace BHive