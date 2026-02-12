#pragma once

#include "core/Core.h"

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>
#include "VulkanMemory.h"
#include "VulkanUtils.h"

struct GLFWwindow;

namespace BHive
{
	struct VkQueueFamilies
	{
		vk::raii::Queue PresentQueue = VK_NULL_HANDLE;
		vk::raii::Queue GraphicsQueue = VK_NULL_HANDLE;
		vk::raii::Queue ComputeQueue = VK_NULL_HANDLE;
		vk::raii::Queue TransferQueue = VK_NULL_HANDLE;

		int32_t PresentQueueIndex = -1;
		int32_t GraphicsQueueIndex = -1;
		int32_t ComputeQueueIndex = -1;
		int32_t TransferQueueIndex = -1;
	};


	class BHIVE_API VulkanBackend
	{
	public:
		using DeviceCallback = std::function<void()>;

		static const uint32_t MINIMUM_VULKAN_API_VERSION = vk::ApiVersion14;

		static const uint32_t MAX_FRAMES_IN_FLIGHT = 2;

	public:
		void Init();

		void Shutdown();

		std::vector<const char *> GetRequiredExtensions();

		uint32_t SelectQueueIndex(vk::QueueFlags queue_type, const vk::SurfaceKHR &surface);

		uint32_t SelectQueueIndex(vk::QueueFlags queue_type);

		void CreateLogicalDevice(const vk::SurfaceKHR &surface);

		void EnsurePresentSupportForSurface(const vk::SurfaceKHR &surface);

		// device lifecycle callbacks
		void RegisterOnDeviceCreated(const DeviceCallback &callback);

		void RegisterOnDeviceDestroy(const DeviceCallback &callback);

		void CallOnDeviceDestroyed();

		static VulkanBackend &Get()
		{
			static VulkanBackend sBackend;
			return sBackend;
		}

		static vk::raii::Device &GetLogicalDevice() { return Get().mLogicalDevice; }

		static vk::raii::PhysicalDevice &GetPhysicalDevice() { return Get().mPhysicalDevice; }

		static const VkQueueFamilies &GetQueueFamilies() { return Get().mQueueFamilies; }

		static vk::raii::Instance &GetInstance() { return Get().mVulkanInstance; }

	private:
		void CreateIntance();

		void CreateDebugMessenger();

		void PickPhysicalDevice();

	private:
		vk::raii::Context mVulkanContext;

		vk::raii::Instance mVulkanInstance = nullptr;

		vk::raii::DebugUtilsMessengerEXT mDebugMessenger = nullptr;

		vk::raii::PhysicalDevice mPhysicalDevice = nullptr;

		vk::raii::Device mLogicalDevice = nullptr;

		VkQueueFamilies mQueueFamilies;

		bool mInitialized = false;

		std::vector<DeviceCallback> mOnDeviceDestroyedCallbacks;

		std::vector<DeviceCallback> mOnDeviceCreatedCallbacks;
	};

	
} // namespace BHive