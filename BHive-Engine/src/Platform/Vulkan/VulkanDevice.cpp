//#include "VulkanDevice.h"
//
//namespace BHive
//{
//	VulkanDevice::~VulkanDevice()
//	{
//		mDevice.waitIdle();
//	}
//
//	void VulkanDevice::Init(vk::raii::PhysicalDevice &physicalDevice, vk::raii::SurfaceKHR &surface)
//	{
//		std::vector<vk::QueueFamilyProperties> queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
//		for (uint32_t qfpIndex = 0; qfpIndex < static_cast<uint32_t>(queueFamilyProperties.size()); qfpIndex++)
//		{
//			if ((queueFamilyProperties[qfpIndex].queueFlags & vk::QueueFlagBits::eGraphics) && physicalDevice.getSurfaceSupportKHR(qfpIndex, *surface))
//			{
//				mQueueFamilies.GraphicsQueueIndex = qfpIndex;
//				break;
//			}
//		}
//
//		if (mQueueFamilies.GraphicsQueueIndex == ~0)
//		{
//			LOG_ERROR("Failed to find a suitable queue family!");
//			ASSERT(false);
//		}
//
//		auto queue_priority = 0.0f;
//		auto requiredDeviceExtensions = GetRequiredExtensions();
//
//		vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan11Features, vk::PhysicalDeviceVulkan13Features, vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT> featureChain;
//		featureChain.assign<vk::PhysicalDeviceFeatures2>({}); // default initialize all features to false
//		featureChain.get<vk::PhysicalDeviceVulkan11Features>().shaderDrawParameters = true;
//		featureChain.get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering = true;
//		featureChain.get<vk::PhysicalDeviceVulkan13Features>().synchronization2 = true;
//		featureChain.get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().extendedDynamicState = true;
//
//		vk::DeviceQueueCreateInfo queueCreateInfo{};
//		queueCreateInfo.queueFamilyIndex = mQueueFamilies.GraphicsQueueIndex;
//		queueCreateInfo.queueCount = 1;
//		queueCreateInfo.pQueuePriorities = &queue_priority;
//
//		vk::DeviceCreateInfo device_createInfo{};
//		device_createInfo.pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>();
//		device_createInfo.queueCreateInfoCount = 1;
//		device_createInfo.pQueueCreateInfos = &queueCreateInfo;
//		device_createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredDeviceExtensions.size());
//		device_createInfo.ppEnabledExtensionNames = requiredDeviceExtensions.data();
//
//		mDevice = physicalDevice.createDevice(device_createInfo);
//
//		mQueueFamilies.GraphicsQueue = mDevice.getQueue(mQueueFamilies.GraphicsQueueIndex, 0);
//	}
//
//	void VulkanDevice::WaitIdle()
//	{
//		mDevice.waitIdle();
//	}
//
//	std::vector<const char *> VulkanDevice::GetRequiredExtensions()
//	{
//		return {vk::KHRSwapchainExtensionName, vk::KHRSpirv14ExtensionName, vk::KHRSynchronization2ExtensionName, vk::KHRCreateRenderpass2ExtensionName};
//	}
//} // namespace BHive