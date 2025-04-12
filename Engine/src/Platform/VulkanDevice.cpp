#include "VulkanContext.h"
#include "VulkanCore.h"
#include "VulkanDevice.h"

namespace BHive
{
	VulkanDevice::VulkanDevice(const VulkanInstance &instance)
	{
		GetPhysicalDevice(instance);
		CreateLogicalDevice();
	}

	VulkanDevice::~VulkanDevice()
	{
		vkDestroyDevice(mLogicalDevice, nullptr);
	}

	const VkQueue &VulkanDevice::GetQueue(const char *name) const
	{
		return mQueues.at(name);
	}

	const VkQueue &VulkanDevice::GetGraphicsQueue() const
	{
		return GetQueue(VK_GRAPHICS_QUEUE_NAME);
	}

	void VulkanDevice::GetPhysicalDevice(const VulkanInstance &instance)
	{
		uint32_t device_count = 0;
		vkEnumeratePhysicalDevices(instance, &device_count, nullptr);

		ASSERT(device_count, "Failed to find GPUs with Vulkan Support ");

		std::vector<VkPhysicalDevice> devices(device_count);
		vkEnumeratePhysicalDevices(instance, &device_count, devices.data());

		std::multimap<int32_t, VkPhysicalDevice> candidates;

		for (auto &device : devices)
		{
			if (!IsDeviceSuitable(device))
				continue;

			auto score = GetDeviceSuitability(device);
			candidates.insert(std::make_pair(score, device));
		}

		if (candidates.rbegin()->first > 0)
		{
			mPhysicalDevice = candidates.rbegin()->second;
		}

		ASSERT(mPhysicalDevice != VK_NULL_HANDLE, "Failed to find suitable GPU!");
	}

	void VulkanDevice::CreateLogicalDevice()
	{
		mQueueFamilyIndices = FindQueueFamilies(mPhysicalDevice);

		// Queue for garphics capabilties
		VkDeviceQueueCreateInfo queuecreateinfo{};
		queuecreateinfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queuecreateinfo.queueFamilyIndex = mQueueFamilyIndices.GraphicsFamily.value();
		queuecreateinfo.queueCount = 1;

		float priority = 1.f;
		queuecreateinfo.pQueuePriorities = &priority;

		VkPhysicalDeviceFeatures features{};
		features.geometryShader = true;

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pQueueCreateInfos = &queuecreateinfo;
		createInfo.queueCreateInfoCount = 1;
		createInfo.pEnabledFeatures = &features;
		createInfo.enabledExtensionCount = 0;

		VK_ASSERT(vkCreateDevice(mPhysicalDevice, &createInfo, nullptr, &mLogicalDevice), "Failed to create logical device");

		VkQueue graphics_queue;
		vkGetDeviceQueue(mLogicalDevice, mQueueFamilyIndices.GraphicsFamily.value(), 0, &graphics_queue);

		mQueues[VK_GRAPHICS_QUEUE_NAME] = graphics_queue;
	}

	int32_t VulkanDevice::GetDeviceSuitability(VkPhysicalDevice device)
	{
		VkPhysicalDeviceProperties properties;
		VkPhysicalDeviceFeatures features;
		vkGetPhysicalDeviceProperties(device, &properties);
		vkGetPhysicalDeviceFeatures(device, &features);

		int32_t score = 0;

		if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			score += 1000;
		}

		score += properties.limits.maxImageDimension2D;

		if (!features.geometryShader)
			return 0;

		return score;
	}

	QueueFamilyIndices VulkanDevice::FindQueueFamilies(VkPhysicalDevice device)
	{
		QueueFamilyIndices indices{};

		uint32_t queue_count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_count, nullptr);

		std::vector<VkQueueFamilyProperties> families(queue_count);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_count, families.data());

		int i = 0;
		for (auto &family : families)
		{
			if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				indices.GraphicsFamily = i;
			}

			i++;
		}

		return indices;
	}

	bool VulkanDevice::IsDeviceSuitable(VkPhysicalDevice device)
	{
		return FindQueueFamilies(device).IsValid();
	}
} // namespace BHive