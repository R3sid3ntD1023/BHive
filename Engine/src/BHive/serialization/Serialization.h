#pragma once

<<<<<<< HEAD
#include "types/unordered_set.hpp"
#include "types/map.hpp"
#include "types/vector.hpp"
#include "types/filesystem.hpp"
#include "types/utility.hpp"
#include "types/string.hpp"
#include "types/optional.hpp"
#include "types/buffer.hpp"
#include "types/aabb.hpp"
#include "types/assethandle.hpp"
#include "types/assetmetadata.hpp"
#include "types/audio.hpp"
#include "types/bone.hpp"
#include "types/color.hpp"
#include "types/enumasbyte.hpp"
#include "types/glm.hpp"
#include "types/keyframe.hpp"
#include "types/lights.hpp"
#include "types/meshdata.hpp"
#include "types/scenecamera.hpp"
#include "types/skeletalnode.hpp"
#include "types/texture.hpp"

=======
#include "serialization/macros.h"
>>>>>>> parent of 7ce9339 (reverted commit)

#include <cereal/types/polymorphic.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/types/optional.hpp>
//#include <cereal/types/string.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/unordered_set.hpp>
#include <cereal/types/base_class.hpp>

#include "types/binarydata.hpp"
#include "types/rttr_type.hpp"
#include "types/filesystem.hpp"
#include "types/uuid.hpp"
#include "types/glm.hpp"
#include "types/aabb.hpp"
#include "types/color.hpp"
#include "types/texture.hpp"
#include "types/assethandle.hpp"
#include "types/enumasbyte.hpp"
#include "types/buffer.hpp"
#include "types/audio.hpp"
#include "types/transform.hpp"
#include "types/lights.hpp"
#include "types/scenecamera.hpp"
#include "types/meshdata.hpp"
#include "types/keyframe.hpp"
#include "types/bone.hpp"
#include "types/skeletalnode.hpp"
#include "types/assetmetadata.hpp"

#include <cereal/archives/json.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/cereal.hpp>

#define CEREAL_BASE(cls, archive) cereal::base_class<cls>(this)
#define MAKE_NVP(name, obj) cereal::make_nvp(name, obj)
