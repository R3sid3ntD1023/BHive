#pragma once

#include "World.h"

namespace BHive
{
    class World;

    class Entity
    {
    public:
        Entity() = default;
        Entity(const entt::entity &handle, World *world);

        operator entt::entity() const
        {
            return mEntityHandle;
        }

        template <typename T, typename... TArgs>
        T *AddComponent(TArgs &&...args)
        {
            ASSERT(!HasComponent<T>());

            return &mWorld->GetRegistry().emplace<T>(mEntityHandle, std::forward<TArgs>(args)...);
        }

        template <typename T, typename... TArgs>
        T *GetComponent() const
        {
            ASSERT(HasComponent<T>());

            return &mWorld->GetRegistry().get<T>(mEntityHandle);
        }

        template <typename T, typename... TArgs>
        bool HasComponent() const
        {
            return mWorld->GetRegistry().any_of<T>(mEntityHandle);
        }

    private:
        World *mWorld = nullptr;
        entt::entity mEntityHandle{entt::null};
    };
} // namespace BHive
