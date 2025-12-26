#include "VulkanCore.h"
#include <GLFW/glfw3.h>

#ifdef _DEBUG
	#define ENABLE_VALIDATION_LAYERS
#endif

namespace BHive
{
	const std::vector<const char *> validationLayers = {
		"VK_LAYER_KHRONOS_validation",
	};

	static VKAPI_ATTR vk::Bool32 VKAPI_CALL
	debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
	{
		switch (messageSeverity)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
			LOG_TRACE("validation layer type: {} - {}", std::to_string(messageType), pCallbackData->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			LOG_INFO("validation layer type: {} - {}", std::to_string(messageType), pCallbackData->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			LOG_WARN("validation layer type: {} - {}", std::to_string(messageType), pCallbackData->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			LOG_ERROR("validation layer type: {} - {}", std::to_string(messageType), pCallbackData->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
			LOG_CRITICAL("validation layer type: {} - {}", std::to_string(messageType), pCallbackData->pMessage);
			break;
		default:
			break;
		}

		return VK_FALSE;
	}

	void VulkanCore::Init()
	{
		if (mInitialized)
			return;

		mInitialized = true;

		CreateIntance();
		CreateDebugMessenger();
		PickPhysicalDevice();
	}

	void VulkanCore::Shutdown()
	{
		if (!mInitialized)
			return;

		mLogicalDevice.waitIdle();
	}

	void VulkanCore::RegisterOnDeviceCreated(const DeviceCallback& callback)
	{
		mOnDeviceCreatedCallbacks.push_back(std::move(callback));
	}

	void VulkanCore::RegisterOnDeviceDestroy(const DeviceCallback& callback)
	{
		mOnDeviceDestroyedCallbacks.push_back(std::move(callback));
	}

	constexpr uint32_t VulkanCore::GetInstanceVersion()
	{
		return vk::ApiVersion14;
	}

	std::vector<const char *> VulkanCore::GetRequiredExtensions()
	{
		return {vk::KHRSwapchainExtensionName, vk::KHRSpirv14ExtensionName, vk::KHRSynchronization2ExtensionName, vk::KHRCreateRenderpass2ExtensionName};
	}

	uint32_t VulkanCore::SelectQueueIndex(vk::QueueFlags queue_type, const vk::SurfaceKHR &surface)
	{
		std::vector<vk::QueueFamilyProperties> queueFamilyProperties = mPhysicalDevice.getQueueFamilyProperties();
		for (uint32_t qfpIndex = 0; qfpIndex < static_cast<uint32_t>(queueFamilyProperties.size()); qfpIndex++)
		{
			if ((queueFamilyProperties[qfpIndex].queueFlags & queue_type) && mPhysicalDevice.getSurfaceSupportKHR(qfpIndex, surface))
			{
				return qfpIndex;
			}
		}

		return ~0;
	}

	uint32_t VulkanCore::SelectQueueIndex(vk::QueueFlags queue_type)
	{
		std::vector<vk::QueueFamilyProperties> queueFamilyProperties = mPhysicalDevice.getQueueFamilyProperties();
		for (uint32_t qfpIndex = 0; qfpIndex < static_cast<uint32_t>(queueFamilyProperties.size()); qfpIndex++)
		{
			if ((queueFamilyProperties[qfpIndex].queueFlags & queue_type))
			{
				return qfpIndex;
			}
		}

		return ~0;
	}

	void VulkanCore::CreateIntance()
	{
		constexpr auto appInfo = vk::ApplicationInfo{"BHive", 1, "No Engine", 1, GetInstanceVersion()};

		uint32_t glfwExtensionCount = 0;
		const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		auto extensionProperties = mVulkanContext.enumerateInstanceExtensionProperties();
		auto layerProperties = mVulkanContext.enumerateInstanceLayerProperties();

		std::vector required_extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
		std::vector<const char *> requiredLayers;

#ifdef ENABLE_VALIDATION_LAYERS
		requiredLayers.assign(validationLayers.begin(), validationLayers.end());
		required_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

#endif

		if (std::ranges::any_of(
				requiredLayers, [layerProperties](const char *layerName)
				{ return std::ranges::none_of(layerProperties, [layerName](const vk::LayerProperties &prop) { return strcmp(prop.layerName, layerName) == 0; }); }))
		{
			LOG_ERROR("Missing required Vulkan validation layers");
			ASSERT(false);
		};

		for (uint32_t i = 0; i < glfwExtensionCount; i++)
		{
			bool found = false;
			if (std::ranges::none_of(extensionProperties, [glfwExtensions, i](const vk::ExtensionProperties &prop) { return strcmp(prop.extensionName, glfwExtensions[i]) == 0; }))
			{
				LOG_ERROR("Missing required Vulkan extension: {}", glfwExtensions[i]);
				ASSERT(false);
			}
		}
		vk::InstanceCreateInfo instanceCreateInfo;
		instanceCreateInfo.pApplicationInfo = &appInfo;
		instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(required_extensions.size());
		instanceCreateInfo.ppEnabledExtensionNames = required_extensions.data();
		instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(requiredLayers.size());
		instanceCreateInfo.ppEnabledLayerNames = requiredLayers.data();

		mVulkanInstance = vk::raii::Instance(mVulkanContext, instanceCreateInfo);
	}

	void VulkanCore::CreateDebugMessenger()
	{
#ifdef ENABLE_VALIDATION_LAYERS

		vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo(
			{}, vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
			vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance, &debugCallback);
		mDebugMessenger = vk::raii::DebugUtilsMessengerEXT(mVulkanInstance, debugCreateInfo);
#endif
	}

	void VulkanCore::PickPhysicalDevice()
	{
		auto devices = mVulkanInstance.enumeratePhysicalDevices();
		const auto devIter = std::ranges::find_if(
			devices,
			[&](const auto &device)
			{
				auto queueFamilies = device.getQueueFamilyProperties();
				bool isSuitable = device.getProperties().apiVersion >= VK_API_VERSION_1_4;
				const auto qfpIter = std::ranges::find_if(queueFamilies, [](const vk::QueueFamilyProperties &qfp) { return (qfp.queueFlags & vk::QueueFlagBits::eGraphics) != (vk::QueueFlags)0; });
				isSuitable = isSuitable && (qfpIter != queueFamilies.end());

				auto extensions = device.enumerateDeviceExtensionProperties();
				bool found = true;
				for (const auto &extension : GetRequiredExtensions())
				{
					auto extensionIter = std::ranges::find_if(extensions, [extension](const auto &ext) { return strcmp(ext.extensionName, extension) == 0; });
					found = found && (extensionIter != extensions.end());
				}

				isSuitable = isSuitable && found;
				if (isSuitable)
				{
					mPhysicalDevice = device;
				}

				return isSuitable;
			});

		if (devIter == devices.end())
		{
			LOG_ERROR("Failed to find a suitable GPU!");
			ASSERT(false);
		}
	}

	vk::raii::SurfaceKHR VulkanCore::CreateSurface(GLFWwindow *window)
	{
		VkSurfaceKHR _surface;
		auto &instance = VulkanCore::GetInstance();
		if (glfwCreateWindowSurface(*instance, window, nullptr, &_surface) != VK_SUCCESS)
		{
			LOG_ERROR("Failed to create window surface!");
			ASSERT(false);
		}

		return  vk::raii::SurfaceKHR(instance, _surface);
	}

	void VulkanCore::EnsurePresentSupportForSurface(const vk::SurfaceKHR &surface)
	{
		if (mLogicalDevice == VK_NULL_HANDLE)
		{
			CreateLogicalDevice(surface);
			return;
		}

		if (mPhysicalDevice.getSurfaceSupportKHR(mQueueFamilies.GraphicsQueueIndex, surface))
		{
			mQueueFamilies.PresentQueueIndex = mQueueFamilies.GraphicsQueueIndex;
			mQueueFamilies.PresentQueue = mQueueFamilies.GraphicsQueue;
			return;
		}

		auto present_family_index = VulkanCore::SelectQueueIndex(vk::QueueFlagBits::eGraphics, surface);
		if (present_family_index == ~0)
		{
			LOG_ERROR("Failed to find a suitable queue family for presenting!");
			ASSERT(false);
		}

		if (present_family_index == mQueueFamilies.PresentQueueIndex)
		{
			return;
		}

		LOG_INFO("Recreating logical device to support presenting on a different queue family.");

		if (mLogicalDevice != VK_NULL_HANDLE)
		{
			try
			{
				mLogicalDevice.waitIdle();		
			}
			catch (...)
			{
			}

			for (const auto &callback : mOnDeviceDestroyedCallbacks)
			{
				callback();
			}
			
			mQueueFamilies.PresentQueue = nullptr;
			mQueueFamilies.GraphicsQueue = nullptr;
			mLogicalDevice = nullptr;
		}

		std::vector<uint32_t> families = {static_cast<uint32_t>(mQueueFamilies.GraphicsQueueIndex), present_family_index};
		std::sort(families.begin(), families.end());
		families.erase(std::unique(families.begin(), families.end()), families.end());

		auto queue_priority = 1.0f;
		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
		for (auto f : families)
		{
			vk::DeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.queueFamilyIndex = f;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queue_priority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		auto requiredDeviceExtensions = VulkanCore::GetRequiredExtensions();

		vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan11Features, vk::PhysicalDeviceVulkan13Features, vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT> featureChain;
		featureChain.assign<vk::PhysicalDeviceFeatures2>({}); // default initialize all features to false
		featureChain.get<vk::PhysicalDeviceVulkan11Features>().shaderDrawParameters = true;
		featureChain.get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering = true;
		featureChain.get<vk::PhysicalDeviceVulkan13Features>().synchronization2 = true;
		featureChain.get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().extendedDynamicState = true;

		vk::DeviceCreateInfo device_createInfo{};
		device_createInfo.pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>();
		device_createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		device_createInfo.pQueueCreateInfos = queueCreateInfos.data();
		device_createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredDeviceExtensions.size());
		device_createInfo.ppEnabledExtensionNames = requiredDeviceExtensions.data();

		mLogicalDevice = mPhysicalDevice.createDevice(device_createInfo);

		mQueueFamilies.GraphicsQueue = mLogicalDevice.getQueue(mQueueFamilies.GraphicsQueueIndex, 0);
		if (present_family_index == mQueueFamilies.GraphicsQueueIndex)
		{
			mQueueFamilies.PresentQueueIndex = mQueueFamilies.GraphicsQueueIndex;
			mQueueFamilies.PresentQueue = mQueueFamilies.GraphicsQueue;
		}
		else
		{
			mQueueFamilies.PresentQueueIndex = present_family_index;
			mQueueFamilies.PresentQueue = mLogicalDevice.getQueue(mQueueFamilies.PresentQueueIndex, 0);
		}

		for (auto &callback : mOnDeviceCreatedCallbacks)
		{
			callback();
		}
	}

	void VulkanCore::CreateLogicalDevice(const vk::SurfaceKHR &surface)
	{
		if (mLogicalDevice != VK_NULL_HANDLE)
		{
			if (!surface)
				return;

			EnsurePresentSupportForSurface(surface);
			return;
		}

		auto graphics_index = VulkanCore::SelectQueueIndex(vk::QueueFlagBits::eGraphics);
		if (graphics_index == ~0)
		{
			LOG_ERROR("Failed to find a suitable queue family!");
			ASSERT(false);
		}

		uint32_t present_index = graphics_index;

		if (surface)
		{
			if (!mPhysicalDevice.getSurfaceSupportKHR(graphics_index, surface))
			{
				auto found = VulkanCore::SelectQueueIndex(vk::QueueFlagBits::eGraphics, surface);
				if (found == ~0)
				{
					LOG_ERROR("Failed to find a suitable queue family for presenting!");
					ASSERT(false);
				}

				present_index = found;
			}
		}

		std::vector<uint32_t> families = {graphics_index, present_index};
		std::sort(families.begin(), families.end());
		families.erase(std::unique(families.begin(), families.end()), families.end());

		auto queue_priority = 1.0f;
		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
		for (auto f : families)
		{
			vk::DeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.queueFamilyIndex = f;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queue_priority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		auto requiredDeviceExtensions = VulkanCore::GetRequiredExtensions();

		vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan11Features, vk::PhysicalDeviceVulkan13Features, vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT> featureChain;
		featureChain.assign<vk::PhysicalDeviceFeatures2>({}); // default initialize all features to false
		featureChain.get<vk::PhysicalDeviceVulkan11Features>().shaderDrawParameters = true;
		featureChain.get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering = true;
		featureChain.get<vk::PhysicalDeviceVulkan13Features>().synchronization2 = true;
		featureChain.get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().extendedDynamicState = true;

		vk::DeviceCreateInfo device_createInfo{};
		device_createInfo.pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>();
		device_createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		device_createInfo.pQueueCreateInfos = queueCreateInfos.data();
		device_createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredDeviceExtensions.size());
		device_createInfo.ppEnabledExtensionNames = requiredDeviceExtensions.data();

		mLogicalDevice = mPhysicalDevice.createDevice(device_createInfo);

		mQueueFamilies.GraphicsQueueIndex = graphics_index;
		mQueueFamilies.GraphicsQueue = mLogicalDevice.getQueue(mQueueFamilies.GraphicsQueueIndex, 0);

		if (families.size() > 1)
		{
			mQueueFamilies.PresentQueueIndex = present_index;
			mQueueFamilies.PresentQueue = mLogicalDevice.getQueue(mQueueFamilies.PresentQueueIndex, 0);
		}
		else
		{
			mQueueFamilies.PresentQueueIndex = mQueueFamilies.GraphicsQueueIndex;
			mQueueFamilies.PresentQueue = mQueueFamilies.GraphicsQueue;
		}
	}
} // namespace BHive