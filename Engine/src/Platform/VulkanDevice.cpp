#include "VulkanContext.h"
#include "VulkanCore.h"
#include "VulkanDevice.h"
#include <GLFW/glfw3.h>

namespace BHive
{
	VulkanDevice::VulkanDevice(const Ref<VulkanInstance> &instance, GLFWwindow *window)
		: mInstance(instance)
	{
		mDeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

		VK_ASSERT(glfwCreateWindowSurface(*instance, window, nullptr, &mSurface), "Failed to create window surface!");
		GetPhysicalDevice(*instance);
		CreateLogicalDevice();
	}

	VulkanDevice::~VulkanDevice()
	{
		vkDestroySurfaceKHR(*mInstance, mSurface, nullptr);
		vkDestroyDevice(mLogicalDevice, nullptr);
	}

	const Queues &VulkanDevice::GetQueues() const
	{
		return mQueues;
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

		std::vector<VkDeviceQueueCreateInfo> queues;
		std::set<uint32_t> queuesFamilies = {mQueueFamilyIndices.Graphics.value(), mQueueFamilyIndices.Present.value()};

		float priority = 1.f;
		for (auto queue : queuesFamilies)
		{
			// Queue for garphics capabilties
			VkDeviceQueueCreateInfo queuecreateinfo{};
			queuecreateinfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queuecreateinfo.queueFamilyIndex = queue;
			queuecreateinfo.queueCount = 1;
			queuecreateinfo.pQueuePriorities = &priority;
			queues.push_back(queuecreateinfo);
		}

		VkPhysicalDeviceFeatures features{};
		features.geometryShader = true;

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pQueueCreateInfos = queues.data();
		createInfo.queueCreateInfoCount = queues.size();
		createInfo.pEnabledFeatures = &features;
		createInfo.enabledExtensionCount = mDeviceExtensions.size();
		createInfo.ppEnabledExtensionNames = mDeviceExtensions.data();

		VK_ASSERT(vkCreateDevice(mPhysicalDevice, &createInfo, nullptr, &mLogicalDevice), "Failed to create logical device");

		vkGetDeviceQueue(mLogicalDevice, mQueueFamilyIndices.Graphics.value(), 0, &mGraphicsQueue);
		vkGetDeviceQueue(mLogicalDevice, mQueueFamilyIndices.Present.value(), 0, &mPresentQueue);
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

	bool VulkanDevice::CheckExtensionSupport(VkPhysicalDevice device, const DeviceExtensions &exts)
	{
		uint32_t extension_count = 0;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);

		std::vector<VkExtensionProperties> extensions(extension_count);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, extensions.data());

		std::set<std::string> requiredExtensions(exts.begin(), exts.end());
		for (const auto &ext : extensions)
		{
			requiredExtensions.erase(ext.extensionName);
		}

		return requiredExtensions.empty();
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
				indices.Graphics = i;
			}

			VkBool32 present_support = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, mSurface, &present_support);

			if (present_support)
			{
				indices.Present = i;
			}

			i++;
		}

		return indices;
	}

	bool VulkanDevice::IsDeviceSuitable(VkPhysicalDevice device)
	{

		return FindQueueFamilies(device).IsValid() && CheckExtensionSupport(device, mDeviceExtensions);
	}
} // namespace BHive