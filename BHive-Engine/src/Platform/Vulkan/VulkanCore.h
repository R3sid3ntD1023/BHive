#pragma once

// #define VULKAN_ERRORS_WITH_ASSERT //engine defined macro for debugging
#define VK_USE_PLATFORM_WIN32_KHR
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan.h>
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace BHive
{
	static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;

	// Vulkan API version your engine requires
	static constexpr uint32_t MINIMUM_VULKAN_API_VERSION = vk::ApiVersion14;

	// Descriptor set indices (global engine convention)
	static constexpr uint32_t GLOBAL_SET_INDEX = 0;
	static constexpr uint32_t MATERIAL_SET_INDEX = 1;
	static constexpr uint32_t OBJECT_SET_INDEX = 2;
	static constexpr uint32_t BATCH_SET_INDEX = 3;
} // namespace BHive

#define VULKAN_ERRORS_WITH_ASSERT 0