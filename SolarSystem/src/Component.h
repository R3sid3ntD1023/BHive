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

#define ADD_COMPONENT_FUNC_NAME "AddComponent"
#define GET_COMPONENT_FUNC_NAME "GetComponent"
#define HAS_COMPONENT_FUNC_NAME "HasComponent"

#define DECLARE_COMPONENT_FUNCS                                               \
	REFLECT_METHOD(ADD_COMPONENT_FUNC_NAME, &CelestrialBody::AddComponent<T>) \
	REFLECT_METHOD(HAS_COMPONENT_FUNC_NAME, &CelestrialBody::HasComponent<T>) \
	REFLECT_METHOD(GET_COMPONENT_FUNC_NAME, &CelestrialBody::GetComponent<T>)