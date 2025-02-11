#pragma once

#include "core/serialization/macros.h"

#include <cereal/types/base_class.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/optional.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/unordered_set.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/types/vector.hpp>

#include "types/binarydata.hpp"
#include "types/buffer.hpp"
#include "types/filesystem.hpp"
#include "types/glm.hpp"

#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>

#define CEREAL_BASE(cls, archive) cereal::base_class<cls>(this)

#define MAKE_BINARY(data, size) cereal::binary_data(data, size)

#define MAKE_NVP_IMPL(name, obj) cereal::make_nvp(name, obj)
#define MAKE_NVP_NAME(name, obj) MAKE_NVP_IMPL(name, obj)
#define MAKE_NVP_NONAME(obj) MAKE_NVP_IMPL(#obj, obj)

#define GET_NVP_MACRO_NAME(arg0, arg1, macro, ...) macro
#define GET_NVP_MACRO(...) EXPAND(GET_NVP_MACRO_NAME(__VA_ARGS__, MAKE_NVP_NAME, MAKE_NVP_NONAME))
#define MAKE_NVP(...) EXPAND(GET_NVP_MACRO(__VA_ARGS__)(__VA_ARGS__))