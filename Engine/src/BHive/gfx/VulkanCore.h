#pragma once

#include "core/Core.h"
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

struct GLFWwindow;

namespace BHive
{
	struct VkQueueFamilies
	{
		vk::raii::Queue PresentQueue = VK_NULL_HANDLE;
		vk::raii::Queue GraphicsQueue = VK_NULL_HANDLE;
		vk::raii::Queue ComputeQueue = VK_NULL_HANDLE;

		int32_t PresentQueueIndex = -1;
		int32_t GraphicsQueueIndex = -1;
		int32_t ComputeQueueIndex = -1;
	};

	struct AllocatedVulkanTexture
	{
		vk::raii::Image Image = VK_NULL_HANDLE;
		vk::raii::ImageView ImageView = VK_NULL_HANDLE;
		vk::raii::DeviceMemory Memory = VK_NULL_HANDLE;
		vk::raii::Sampler Sampler = VK_NULL_HANDLE;
	};

	struct AllocatedVulkanBuffer
	{
		vk::raii::Buffer Buffer = VK_NULL_HANDLE;
		vk::raii::DeviceMemory Memory = VK_NULL_HANDLE;

		void SetData(const void *data, size_t size, uint32_t offset = 0)
		{
			void *mapped = Memory.mapMemory(offset, size);
			memcpy(mapped, data, size);
			Memory.unmapMemory();
		}
	};

	class BHIVE_API VulkanCore
	{
	public:
		using DeviceCallback = std::function<void()>;

		static constexpr uint32_t MINIMUM_VULKAN_API_VERSION = vk::ApiVersion14;

		static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;

	public:
		static void Init();

		static void Shutdown();

		static const vk::raii::Instance &GetInstance() { return mVulkanInstance; }

		static const vk::raii::PhysicalDevice &GetPhysicalDevice() { return mPhysicalDevice; }

		static vk::raii::Device &GetLogicalDevice() { return mLogicalDevice; }

		static std::vector<const char *> GetRequiredExtensions();

		static uint32_t SelectQueueIndex(vk::QueueFlags queue_type, const vk::SurfaceKHR &surface);

		static uint32_t SelectQueueIndex(vk::QueueFlags queue_type);

		static vk::raii::SurfaceKHR CreateSurface(GLFWwindow *window);

		static void CreateLogicalDevice(const vk::SurfaceKHR &surface);

		static const VkQueueFamilies &GetQueueFamilies() { return mQueueFamilies; };

		static void EnsurePresentSupportForSurface(const vk::SurfaceKHR &surface);

		// device lifecycle callbacks
		static void RegisterOnDeviceCreated(const DeviceCallback &callback);

		static void RegisterOnDeviceDestroy(const DeviceCallback &callback);

	private:
		static void CreateIntance();

		static void CreateDebugMessenger();

		static void PickPhysicalDevice();

		static void CreateLogicalDevice();

	private:
		static inline vk::raii::Context mVulkanContext;

		static inline vk::raii::Instance mVulkanInstance = nullptr;

		static inline vk::raii::DebugUtilsMessengerEXT mDebugMessenger = nullptr;

		static inline vk::raii::PhysicalDevice mPhysicalDevice = nullptr;

		static inline vk::raii::Device mLogicalDevice = nullptr;

		static inline VkQueueFamilies mQueueFamilies;

		static inline bool mInitialized = false;

		static inline std::vector<DeviceCallback> mOnDeviceDestroyedCallbacks;

		static inline std::vector<DeviceCallback> mOnDeviceCreatedCallbacks;
	};
} // namespace BHive