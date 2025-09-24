#include "GraphicsContext.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <vulkan/vulkan_to_string.hpp>

namespace BHive
{
	constexpr bool enabledValidateLayers = true;
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

	GraphicsContext::GraphicsContext(GLFWwindow *window)
		: mWindowHandle(window)
	{
	}

	void GraphicsContext::Init()
	{

		constexpr auto appInfo = vk::ApplicationInfo{"BHive", 1, "No Engine", 1, vk::ApiVersion14};

		uint32_t glfwExtensionCount = 0;
		const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		auto extensionProperties = mVulkanContext.enumerateInstanceExtensionProperties();
		auto layerProperties = mVulkanContext.enumerateInstanceLayerProperties();

		std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
		std::vector<const char *> requiredLayers;
		if (enabledValidateLayers)
		{
			requiredLayers.assign(validationLayers.begin(), validationLayers.end());
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

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
		vk::InstanceCreateInfo instanceCreateInfo({}, &appInfo, requiredLayers, extensions);

		mVulkanInstance = vk::raii::Instance(mVulkanContext, instanceCreateInfo);
#if defined(_DEBUG)

		if (enabledValidateLayers)
		{

			vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo(
				{}, vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
				vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance, &debugCallback);
			mDebugMessenger = vk::raii::DebugUtilsMessengerEXT(mVulkanInstance, debugCreateInfo);
		}
#endif

		auto devices = mVulkanInstance.enumeratePhysicalDevices();
		if (devices.empty())
		{
			LOG_CRITICAL("Failed to find GPUs with Vulkan support!");
			ASSERT(false);
		}

		std::multimap<int, vk::raii::PhysicalDevice> candidates;
		for (auto &device : devices)
		{
			auto props = device.getProperties();
			auto features = device.getFeatures();
			LOG_INFO("Found device: {} (type: {})", props.deviceName, vk::to_string(props.deviceType));
			uint32_t score = 0;

			if (props.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
			{
				score += 1000;
			}

			score += props.limits.maxImageDimension2D;
			if (!features.geometryShader)
			{
				score = 0;
			}
			candidates.insert(std::make_pair(score, device));

			if (candidates.rbegin()->first > 0)
			{
				mPhysicalDevice = CreateScope<vk::raii::PhysicalDevice>(candidates.rbegin()->second);
			}
			else
			{
				LOG_CRITICAL("Failed to find a suitable GPU!");
				ASSERT(false);
			}
		}
	}

	void GraphicsContext::SwapBuffers()
	{
		// glfwSwapBuffers(mWindowHandle);
	}

} // namespace BHive