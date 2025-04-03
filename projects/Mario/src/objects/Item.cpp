#include "Item.h"
#include "components/BoxComponent.h"

namespace BHive
{
	Item::Item(const entt::entity &handle, World *world)
		: Block(handle, world)
	{
		GetComponent<BoxComponent>()->OnTriggerEnter.bind(this, &Item::OnCollisionTriggerEnter);
	}

	void Item::OnCollisionTriggerEnter(ColliderComponent *component, GameObject *other)
	{
	}
} // namespace BHive