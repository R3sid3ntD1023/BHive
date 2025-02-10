#pragma once

#include <core/Core.h>
#include <core/reflection/Reflection.h>
#include <core/serialization/Serialization.h>

struct Component
{
	virtual void Save(cereal::JSONOutputArchive &ar) const {};

	virtual void Load(cereal::JSONInputArchive &ar) {};

	REFLECTABLEV()
};

void Save(cereal::JSONOutputArchive &ar, const Ref<Component> &obj);
void Load(cereal::JSONInputArchive &ar, Ref<Component> &obj);

REFLECT(Component)
{
	BEGIN_REFLECT(Component);
}