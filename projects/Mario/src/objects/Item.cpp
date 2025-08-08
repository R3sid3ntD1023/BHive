#include "components/BoxColliderComponent.h"
#include "Item.h"

namespace BHive
{
	Item::Item(const entt::entity &handle, World *world)
		: Block(handle, world)
	{
		GetComponent<BoxColliderComponent>()->OnTriggerEnter.bind(this, &Item::OnCollisionTriggerEnter);
	}

	void Item::OnCollisionTriggerEnter(ColliderComponent *component, GameObject *other)
	{
	}

	RTTR_REGISTRATION
	{
		BEGIN_REFLECT(Item)
		REFLECT_CONSTRUCTOR(const entt::entity &, World *);
	}
} // namespace BHive