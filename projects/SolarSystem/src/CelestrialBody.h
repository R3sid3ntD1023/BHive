#pragma once

#include "Component.h"
#include "Universe.h"
#include <core/Core.h>
#include <core/reflection/Reflection.h>
#include <core/serialization/Serialization.h>
#include <math/Transform.h>

BEGIN_NAMESPACE(BHive)
class Shader;
class Universe;
struct IDComponent;
struct TagComponent;

struct CelestrialBody
{
	CelestrialBody(Universe *universe);

	virtual void Begin();

	virtual void Update(float dt);

	void SetName(const std::string &name);

	void SetParent(const BHive::UUID &parent);

	BHive::FTransform GetTransform() const;
	BHive::FTransform &GetLocalTransform() { return mTransform; }

	virtual void Save(cereal::JSONOutputArchive &ar) const;

	virtual void Load(cereal::JSONInputArchive &ar);

	template <typename T, typename... TArgs>
	Ref<T> AddComponent(TArgs &&...args)
	{
		auto component = CreateRef<T>(std::forward<TArgs>(args)...);
		AddComponent(component);
		return component;
	}

	void AddComponent(const Ref<Component> &component);

	REFLECTABLEV()

protected:
	BHive::FTransform mTransform;

private:
	BHive::UUID mParent = NullID;
	Universe *mUniverse = nullptr;
	std::vector<Ref<Component>> mComponents;
	std::vector<Ref<Component>> mTickedComponents;

	Ref<IDComponent> mIDComponent;
	Ref<TagComponent> mTagComponent;
};

END_NAMESPACE