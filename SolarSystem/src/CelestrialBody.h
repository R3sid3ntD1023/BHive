#pragma once

#include "Component.h"
#include <core/Core.h>
#include <core/reflection/Reflection.h>
#include <core/serialization/Serialization.h>
#include <entt/entt.hpp>
#include <math/Transform.h>
#include "Universe.h"

namespace BHive
{
	class Shader;
} // namespace BHive

class Universe;

struct CelestrialBody
{
	CelestrialBody(const entt::entity &entity, Universe *universe);

	void SetParent(const BHive::UUID &parent);

	BHive::FTransform GetTransform() const;
	BHive::FTransform &GetLocalTransform() { return mTransform; }

	virtual void Save(cereal::JSONOutputArchive &ar) const;

	virtual void Load(cereal::JSONInputArchive &ar);

	template <typename T, typename... TArgs>
	T *AddComponent(TArgs &&...args)
	{
		ASSERT(!HasComponent<T>());

		return &mUniverse->GetRegistry().emplace<T>(mEntityHandle, std::forward<TArgs>(args)...);
	}

	template <typename T>
	T *GetComponent()
	{
		ASSERT(HasComponent<T>());

		return &mUniverse->GetRegistry().get<T>(mEntityHandle);
	}

	template <typename T>
	bool HasComponent() const
	{
		return mUniverse->GetRegistry().any_of<T>(mEntityHandle);
	}

	REFLECTABLEV()

protected:
	BHive::FTransform mTransform;

private:
	BHive::UUID mParent = 0;
	entt::entity mEntityHandle{entt::null};
	Universe *mUniverse = nullptr;
};
