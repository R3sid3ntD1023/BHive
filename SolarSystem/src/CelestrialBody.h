#pragma once

#include "Component.h"
#include <core/Core.h>
#include <core/reflection/Reflection.h>
#include <core/serialization/Serialization.h>
#include <entt/entt.hpp>
#include <math/Transform.h>

namespace BHive
{
	class Shader;
} // namespace BHive

class Universe;

struct CelestrialBody
{
	CelestrialBody(const entt::entity &entity, Universe *universe);

	void Update(const Ref<BHive::Shader> &shader, float);

	virtual void OnUpdate(const Ref<BHive::Shader> &shader, float) {}

	BHive::FTransform GetTransform() const;
	const BHive::FTransform &GetLocalTransform() const { return mTransform; }

	virtual void Save(cereal::JSONOutputArchive &ar) const;

	virtual void Load(cereal::JSONInputArchive &ar);

	REFLECTABLEV()

protected:
	BHive::FTransform mTransform;

private:
	BHive::UUID mParent = 0;
	std::vector<Ref<Component>> mComponents;

	entt::entity mEntityHandle{entt::null};
	Universe *mUniverse = nullptr;
};
