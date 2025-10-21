#pragma once

#include "core/Core.h"
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace BHive
{
	class BHIVE_API VulkanCore
	{
	public:
		static void Init();

		static void Shutdown();

		static const vk::raii::Instance &GetInstance() { return mVulkanInstance; }

		static const vk::raii::PhysicalDevice &GetPhysicalDevice() { return mPhysicalDevice; }

		static constexpr uint32_t GetInstanceVersion();

		static std::vector<const char *> GetRequiredExtensions();

		static uint32_t SelectQueueIndex(vk::QueueFlags queue_type, const vk::SurfaceKHR &surface);

	private:
		static void CreateIntance();

		static void CreateDebugMessenger();

		static void PickPhysicalDevice();

	private:
		static inline vk::raii::Context mVulkanContext;

		static inline vk::raii::Instance mVulkanInstance = nullptr;

		static inline vk::raii::DebugUtilsMessengerEXT mDebugMessenger = nullptr;

		static inline vk::raii::PhysicalDevice mPhysicalDevice = nullptr;

		static inline bool mInitialized = false;
	};
} // namespace BHive