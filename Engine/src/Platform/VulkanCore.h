#include "core/Core.h"
#include <vulkan/vulkan.h>

#define VK_TYPE(type) VK_STRUCTURE_TYPE_##type
#define VK_ASSERT(function, ...) ASSERT(function == VK_SUCCESS, __VA_ARGS__)