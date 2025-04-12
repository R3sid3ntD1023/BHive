#pragma once

#include "VulkanCore.h"
#include "VulkanInstance.h"

#define VK_GRAPHICS_QUEUE_NAME "GRAPHICS"

struct GLFWwindow;

namespace BHive
{
	struct QueueFamilyIndices
	{
		std::optional<uint32_t> Graphics;
		std::optional<uint32_t> Present;

		bool IsValid() const { return Graphics.has_value() && Present.has_value(); }
	};

	using Queues = std::unordered_set<VkQueue>;
	using DeviceExtensions = std::vector<const char *>;

	class VulkanDevice
	{
	public:
		VulkanDevice(const Ref<VulkanInstance> &instance, GLFWwindow *window);
		~VulkanDevice();

		operator VkDevice() const { return mLogicalDevice; }
		operator VkSurfaceKHR() const { return mSurface; }

		const Queues &GetQueues() const;

	private:
		void GetPhysicalDevice(const VulkanInstance &instance);
		void CreateLogicalDevice();
		int32_t GetDeviceSuitability(VkPhysicalDevice device);
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
		bool IsDeviceSuitable(VkPhysicalDevice device);
		bool CheckExtensionSupport(VkPhysicalDevice device, const DeviceExtensions &exts);

	private:
		VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
		VkDevice mLogicalDevice = VK_NULL_HANDLE;
		VkSurfaceKHR mSurface = VK_NULL_HANDLE;
		VkQueue mGraphicsQueue = VK_NULL_HANDLE;
		VkQueue mPresentQueue = VK_NULL_HANDLE;

		QueueFamilyIndices mQueueFamilyIndices;
		Queues mQueues;
		DeviceExtensions mDeviceExtensions;

		Ref<VulkanInstance> mInstance;
	};
} // namespace BHive