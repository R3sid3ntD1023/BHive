#pragma once

// #define VULKAN_ERRORS_WITH_ASSERT //engine defined macro for debugging
#define VK_USE_PLATFORM_WIN32_KHR
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace BHive
{
	static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;

	// Vulkan API version your engine requires
	static constexpr uint32_t MINIMUM_VULKAN_API_VERSION = vk::ApiVersion14;

} // namespace BHive

#define BHIVE_ENABLE_OBJECT_NAMES 1