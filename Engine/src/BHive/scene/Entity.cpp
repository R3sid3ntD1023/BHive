#include "Entity.h"
#include "World.h"

namespace BHive
{
    Entity::Entity(const entt::entity &handle, World *world)
        : mWorld(world), mEntityHandle(handle)
    {
    }
} // namespace BHive
