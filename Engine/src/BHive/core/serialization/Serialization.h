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
#define MAKE_NVP(name, obj) cereal::make_nvp(name, obj)
#define MAKE_BINARY(data, size) cereal::binary_data(data, size)
