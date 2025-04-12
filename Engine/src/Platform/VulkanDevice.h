#pragma once

#include "VulkanCore.h"
#include "VulkanInstance.h"

#define VK_GRAPHICS_QUEUE_NAME "GRAPHICS"

namespace BHive
{
	struct QueueFamilyIndices
	{
		std::optional<uint32_t> GraphicsFamily;

		bool IsValid() const { return GraphicsFamily.has_value(); }
	};

	class VulkanDevice
	{
	public:
		VulkanDevice(const VulkanInstance &instance);
		~VulkanDevice();

		operator VkDevice() const { return mLogicalDevice; }

		const VkQueue &GetQueue(const char *) const;
		const VkQueue &GetGraphicsQueue() const;

	private:
		void GetPhysicalDevice(const VulkanInstance &instance);
		void CreateLogicalDevice();
		int32_t GetDeviceSuitability(VkPhysicalDevice device);
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
		bool IsDeviceSuitable(VkPhysicalDevice device);

	private:
		VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
		VkDevice mLogicalDevice = VK_NULL_HANDLE;
		QueueFamilyIndices mQueueFamilyIndices;
		std::unordered_map<const char *, VkQueue> mQueues;
	};
} // namespace BHive