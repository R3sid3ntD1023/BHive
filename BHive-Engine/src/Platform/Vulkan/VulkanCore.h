#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

#define MAX_FRAMES_IN_FLIGHT 2
#define MINIMUM_VULKAN_API_VERSION vk::ApiVersion14