#pragma once

#include "World.h"

namespace BHive
{

	struct GameObject
	{
		GameObject(const std::string &name, const entt::entity &handle, World *world);

		template <typename T, typename... TArgs>
		T &AddComponent(TArgs &&...args)
		{
			return mWorld->mRegistry.emplace<T>(mEntity, std::forward<TArgs>(args)...);
		}

		template <typename T>
		const T &GetComponent() const
		{
			return mWorld->mRegistry.get<T>(mEntity);
		}

		template <typename T>
		T &GetComponent()
		{
			return mWorld->mRegistry.get<T>(mEntity);
		}

		const UUID &GetID() const;

		const FTransform &GetTransform() const;

	private:
		entt::entity mEntity{entt::null};
		World *mWorld = nullptr;
	};
} // namespace BHive