#include "VulkanCore.h"
#include "VulkanInstance.h"
#include <glfw/glfw3.h>

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

	VulkanInstance::VulkanInstance(const char *name, const char *engine_name)
	{
		mValidationLayers = {"VK_LAYER_KHRONOS_validation"};

		VkApplicationInfo appinfo{};
		appinfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appinfo.pApplicationName = name;
		appinfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appinfo.pEngineName = engine_name;
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

#ifdef _DEBUG
		CreateDebugMessenger();
#endif // _DEBUG
	}

	VulkanInstance::~VulkanInstance()
	{
#ifdef _DEBUG
		DestroyDebugUtilsMessengerEXT(mInstance, mDebugMessenger, nullptr);
#endif // _DEBUG

		vkDestroyInstance(mInstance, nullptr);
	}

	bool VulkanInstance::CheckValidationLayerSupport()
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

	std::vector<const char *> VulkanInstance::GetRequiredExtensions()
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
	void VulkanInstance::CreateDebugMessenger()
	{
		VkDebugUtilsMessengerCreateInfoEXT createInfo{};
		PopulateDebugCreateInfo(createInfo);

		VK_ASSERT(
			CreateDebugUtilsMessengerEXT(mInstance, &createInfo, nullptr, &mDebugMessenger), "Failed to setup debugger!");
	}

	void VulkanInstance::PopulateDebugCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo)
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
} // namespace BHive