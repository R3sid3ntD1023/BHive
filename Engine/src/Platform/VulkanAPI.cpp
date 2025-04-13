#include "core/Application.h"
#include "core/Window.h"
#include "VulkanAPI.h"
#include <GLFW/glfw3.h>

namespace BHive
{
	static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
		VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type,
		const VkDebugUtilsMessengerCallbackDataEXT *callback, void *userData)
	{
		switch (severity)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
			LOG_TRACE("Vulkan:: {}", callback->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			LOG_INFO("Vulkan:: {}", callback->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			LOG_WARN("Vulkan:: {}", callback->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			LOG_ERROR("Vulkan:: {}", callback->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
			break;
		default:
			break;
		}
		return VK_FALSE;
	}

	VkResult CreateDebugUtilsMessengerEXT(
		VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator,
		VkDebugUtilsMessengerEXT *pDebugMessenger)
	{
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr)
		{
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else
		{
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	void DestroyDebugUtilsMessengerEXT(
		VkInstance instance, VkDebugUtilsMessengerEXT pDebugMessenger, const VkAllocationCallbacks *pAllocator)
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr)
		{
			func(instance, pDebugMessenger, pAllocator);
		}
	}

	void VulkanAPI::Init(const char *name, GLFWwindow *window)
	{
		mDeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

		CreateInstance(name);
		CreateDebugMessenger();
		CreateWindowSurface(window);
		GetPhysicalDevice();
		CreateLogialDevice();
	}

	void VulkanAPI::Shutdown()
	{
#ifdef _DEBUG
		DestroyDebugUtilsMessengerEXT(mInstance, mDebugMessenger, nullptr);
#endif // _DEBUG

		vkDestroyDevice(mDevice, nullptr);

		vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
		vkDestroyInstance(mInstance, nullptr);
	}

	void VulkanAPI::CreateInstance(const char *name)
	{
		mValidationLayers = {"VK_LAYER_KHRONOS_validation"};

		VkApplicationInfo appinfo{};
		appinfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appinfo.pApplicationName = name;
		appinfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appinfo.pEngineName = name;
		appinfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appinfo.apiVersion = VK_API_VERSION_1_3;

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appinfo;

		auto extensions = GetRequiredExtensions();
		createInfo.enabledExtensionCount = extensions.size();
		createInfo.ppEnabledExtensionNames = extensions.data();

#ifdef _DEBUG
		ASSERT(CheckValidationLayerSupport(), "Validation Layers requested but not found!");

		createInfo.enabledLayerCount = mValidationLayers.size();
		createInfo.ppEnabledLayerNames = mValidationLayers.data();

		VkDebugUtilsMessengerCreateInfoEXT debugcreateInfo{};
		PopulateDebugCreateInfo(debugcreateInfo);
		createInfo.pNext = &debugcreateInfo;
#else
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
#endif // _DEBUG

		VK_ASSERT(vkCreateInstance(&createInfo, nullptr, &mInstance), "Failed to create instance!");
	}

	void VulkanAPI::CreateLogialDevice()
	{

		mQueueFamilyIndices = FindQueueFamilies(mPhysicalDevice);
		mSwapChainSupportDetails = QuerySwapChainSupportDetails(mPhysicalDevice);

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

		VK_ASSERT(vkCreateDevice(mPhysicalDevice, &createInfo, nullptr, &mDevice), "Failed to create logical device");

		vkGetDeviceQueue(mDevice, mQueueFamilyIndices.Graphics.value(), 0, &mGraphicsQueue);
		vkGetDeviceQueue(mDevice, mQueueFamilyIndices.Present.value(), 0, &mPresentQueue);
	}

	void VulkanAPI::GetPhysicalDevice()
	{
		uint32_t device_count = 0;
		vkEnumeratePhysicalDevices(mInstance, &device_count, nullptr);

		ASSERT(device_count, "Failed to find GPUs with Vulkan Support ");

		std::vector<VkPhysicalDevice> devices(device_count);
		vkEnumeratePhysicalDevices(mInstance, &device_count, devices.data());

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

	void VulkanAPI::CreateWindowSurface(GLFWwindow *window)
	{
		VK_ASSERT(glfwCreateWindowSurface(mInstance, window, nullptr, &mSurface), "Failed to create window surface!");
	}

	bool VulkanAPI::CheckValidationLayerSupport()
	{
		uint32_t layer_count = 0;
		vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
		std::vector<VkLayerProperties> layers(layer_count);
		vkEnumerateInstanceLayerProperties(&layer_count, layers.data());

		for (auto name : mValidationLayers)
		{
			bool found = false;
			for (const auto &layer : layers)
			{
				if (strcmp(name, layer.layerName) == 0)
				{
					found = true;
					break;
				}
			}

			if (!found)
				return false;
		}

		return true;
	}

	void VulkanAPI::PopulateDebugCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo)
	{
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
									 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
									 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
								 VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
								 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debug_callback;
		createInfo.pUserData = nullptr;
	}

	std::vector<const char *> VulkanAPI::GetRequiredExtensions()
	{
		uint32_t extension_count = 0;
		const char **glfwExtensions;

		glfwExtensions = glfwGetRequiredInstanceExtensions(&extension_count);

		std::vector<const char *> extensions(glfwExtensions, glfwExtensions + extension_count);

#ifdef _DEBUG
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif // _DEBUG

		return extensions;
	}

	void VulkanAPI::CreateDebugMessenger()
	{
#if _DEBUG

		VkDebugUtilsMessengerCreateInfoEXT createInfo{};
		PopulateDebugCreateInfo(createInfo);

		VK_ASSERT(
			CreateDebugUtilsMessengerEXT(mInstance, &createInfo, nullptr, &mDebugMessenger), "Failed to setup debugger!");
#endif // _DEBUG
	}

	int32_t VulkanAPI::GetDeviceSuitability(VkPhysicalDevice device)
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

	QueueFamilyIndices VulkanAPI::FindQueueFamilies(VkPhysicalDevice device)
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

	bool VulkanAPI::IsDeviceSuitable(VkPhysicalDevice device)
	{
		bool swap_chain_adequate = false;
		bool extensions_supported = CheckExtensionSupport(device, mDeviceExtensions);

		if (extensions_supported)
		{
			SwapChainSupportDetails details = QuerySwapChainSupportDetails(device);
			swap_chain_adequate = details.Formats.size() && details.PresentModes.size();
		}

		return FindQueueFamilies(device).IsValid() && extensions_supported && swap_chain_adequate;
	}

	bool VulkanAPI::CheckExtensionSupport(VkPhysicalDevice device, const DeviceExtensions &exts)
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

	SwapChainSupportDetails VulkanAPI::QuerySwapChainSupportDetails(VkPhysicalDevice device)
	{
		SwapChainSupportDetails details;
		uint32_t format_count = 0;
		uint32_t present_mode_count = 0;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, mSurface, &details.Capabilities);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, mSurface, &format_count, nullptr);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, mSurface, &present_mode_count, nullptr);

		if (format_count)
		{
			details.Formats.resize(format_count);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, mSurface, &format_count, details.Formats.data());
		}

		if (present_mode_count)
		{
			details.PresentModes.resize(present_mode_count);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, mSurface, &present_mode_count, details.PresentModes.data());
		}

		return details;
	}

	Scope<VulkanAPI> VulkanAPI::sAPI = CreateScope<VulkanAPI>();
} // namespace BHive