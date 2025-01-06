#pragma once

#include "ComponentMacros.h"

#define INPUT_ARCHIVE cereal::JSONInputArchive
#define OUTPUT_ARCHIVE cereal::JSONOutputArchive

// #define HAS_COMPONENT_FUNC HAS_COMPONENT_FUNC_NAME, entity::HasComponent<T>
//  #define GET_COMPONENT_FUNC GET_COMPONENT_FUNC_NAME, entity::GetComponent<T>

// #define SERIALIZE_COMPONENT_FUNC SERIALIZE_COMPONENT_FUNC_NAME, &::BHive::SerializeComponent<OUTPUT_ARCHIVE, T>
// #define DESERIALIZE_COMPONENT_FUNC DESERIALIZE_COMPONENT_FUNC_NAME, &::BHive::DeserializeComponent<INPUT_ARCHIVE, T>

#define REQUIRED_COMPONENT_FUNCS() \
    REFLECT_CONSTRUCTOR()          \
    REFLECT_CONSTRUCTOR(const T &)\
    REFLECT_METHOD(DUPLICATE_COMPONENT_FUNC_NAME, ::BHive::DuplicateComponent<T>)\
    REFLECT_METHOD(COPY_COMPONENT_FUNC_NAME, ::BHive::CopyComponent<T>)

namespace BHive
{
    class Entity;

}