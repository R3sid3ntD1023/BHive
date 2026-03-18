#include "VulkanBackend.h"
#include "core/debug/CrashHandler.h"
#include "VulkanUtils.h"
#include <GLFW/glfw3.h>

#ifdef _DEBUG
	#define VALIDATION_LAYERS_ENABLED
	#ifdef VALIDATION_LAYERS_ENABLED
		#define ENABLE_VALIDATION_LAYERS
	#endif
#endif

namespace BHive
{
	static const std::vector<const char *> s_validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

	static VKAPI_ATTR vk::Bool32 VKAPI_CALL
	debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
	{
		auto message_type = static_cast<vk::DebugUtilsMessageTypeFlagsEXT>(messageType);
		auto message_type_string = vk::to_string(message_type);

		switch (messageSeverity)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
			LOG_TRACE("{} : {}", message_type_string, pCallbackData->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			LOG_INFO("{} : {}", message_type_string, pCallbackData->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			LOG_WARN("{} : {}", message_type_string, pCallbackData->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			LOG_ERROR("{} : {}", message_type_string, pCallbackData->pMessage);
			ASSERT(false)
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
			LOG_CRITICAL("{} : {}", message_type_string, pCallbackData->pMessage);
			break;
		default:
			break;
		}

		return false;
	}

	namespace details
	{
		auto GetPhysicalDeviceFeaturesChain()
		{

			vk::PhysicalDeviceFeatures features{};
			features.setFillModeNonSolid(true).setWideLines(true);

			vk::StructureChain<
				vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan11Features, vk::PhysicalDeviceVulkan12Features, vk::PhysicalDeviceVulkan13Features,
				vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT, vk::PhysicalDeviceVertexInputDynamicStateFeaturesEXT, vk::PhysicalDeviceVertexAttributeDivisorFeaturesEXT,
				vk::PhysicalDeviceRobustness2FeaturesKHR>
				featureChain;
			featureChain.assign<vk::PhysicalDeviceFeatures2>({}); // default initialize all features to false
			
			featureChain.get<vk::PhysicalDeviceFeatures2>().setFeatures(features);
			featureChain.get<vk::PhysicalDeviceVulkan11Features>().setShaderDrawParameters(true);
			featureChain.get<vk::PhysicalDeviceVulkan12Features>()
				.setDescriptorBindingSampledImageUpdateAfterBind(true)
				.setDescriptorBindingPartiallyBound(true)
				.setDescriptorBindingUpdateUnusedWhilePending(true)
				.setDescriptorBindingStorageBufferUpdateAfterBind(true)
				.setDescriptorBindingUniformBufferUpdateAfterBind(true);
			featureChain.get<vk::PhysicalDeviceVulkan13Features>().setDynamicRendering(true).setSynchronization2(true).setDescriptorBindingInlineUniformBlockUpdateAfterBind(true);
			featureChain.get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().setExtendedDynamicState(true);
			featureChain.get<vk::PhysicalDeviceVertexInputDynamicStateFeaturesEXT>().setVertexInputDynamicState(true);
			featureChain.get<vk::PhysicalDeviceVertexAttributeDivisorFeaturesEXT>().setVertexAttributeInstanceRateZeroDivisor(true);
			featureChain.get<vk::PhysicalDeviceRobustness2FeaturesKHR>().setNullDescriptor(true).setRobustImageAccess2(true);

			return featureChain;
		}
	} // namespace details

	void VulkanBackend::Init()
	{

		CreateIntance();
		CreateDebugMessenger();
		PickPhysicalDevice();

		auto log_info = [=](std::ofstream &log)
		{
			if (mPhysicalDevice != VK_NULL_HANDLE)
			{
				auto props = mPhysicalDevice.getProperties();
				log << "GPU: " << props.deviceName << "\n";
				log << "Driver Version: " << props.driverVersion << "\n";
				log << "Vulkan API Version: " << VK_VERSION_MAJOR(props.apiVersion) << "." << VK_VERSION_MINOR(props.apiVersion) << "." << VK_VERSION_PATCH(props.apiVersion) << "\n";
			}
			else
			{
				log << "No Vulkan physical device information available\n"; 
			}
		};
			

		CrashHandler::Get().SetLogInfo(log_info);
	}

	void VulkanBackend::Shutdown()
	{
		mGPUResourceManager->Shutdown();
	}

	void VulkanBackend::RegisterOnDeviceCreated(const DeviceCallback &callback)
	{
		mOnDeviceCreatedCallbacks.push_back(callback);
	}

	void VulkanBackend::RegisterOnDeviceDestroy(const DeviceCallback &callback)
	{
		mOnDeviceDestroyedCallbacks.push_back(callback);
	}

	void VulkanBackend::CallOnDeviceDestroyed()
	{
		for (auto &callback : mOnDeviceDestroyedCallbacks)
			callback();
	}

	std::vector<const char *> VulkanBackend::GetRequiredExtensions()
	{
		return {
			vk::KHRSwapchainExtensionName,
			vk::KHRSpirv14ExtensionName,
			vk::KHRSynchronization2ExtensionName,
			vk::KHRCreateRenderpass2ExtensionName,
			vk::EXTVertexInputDynamicStateExtensionName,
			vk::EXTExtendedDynamicStateExtensionName,
			vk::EXTVertexAttributeDivisorExtensionName};
	}

	uint32_t VulkanBackend::SelectQueueIndex(vk::QueueFlags queue_type, const vk::SurfaceKHR &surface)
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

	uint32_t VulkanBackend::SelectQueueIndex(vk::QueueFlags queue_type)
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

	void VulkanBackend::CreateIntance()
	{
		constexpr auto appInfo = vk::ApplicationInfo{"BHive", 1, "BHiveEngine", 1, MINIMUM_VULKAN_API_VERSION};

		uint32_t glfwExtensionCount = 0;
		const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		auto extensionProperties = mVulkanContext.enumerateInstanceExtensionProperties();
		auto layerProperties = mVulkanContext.enumerateInstanceLayerProperties();

		std::vector required_extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
		std::vector<const char *> enabled_layers;

#ifdef ENABLE_VALIDATION_LAYERS
		enabled_layers.assign(s_validationLayers.begin(), s_validationLayers.end());
		required_extensions.push_back(vk::EXTDebugUtilsExtensionName);
		required_extensions.push_back(vk::EXTValidationFeaturesExtensionName);
#endif

		if (std::ranges::any_of(
				enabled_layers, [layerProperties](const char *layerName)
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

		

#ifdef ENABLE_VALIDATION_LAYERS

		vk::ValidationFeatureEnableEXT enabled_features[] =
			{
				vk::ValidationFeatureEnableEXT::eBestPractices, 
				vk::ValidationFeatureEnableEXT::eSynchronizationValidation, 
				vk::ValidationFeatureEnableEXT::eDebugPrintf,
				vk::ValidationFeatureEnableEXT::eGpuAssisted
			};

		vk::ValidationFeaturesEXT enabled(enabled_features);
	
		auto loglevels = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
						 vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo;
		auto messageTypes = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;

		vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo({}, loglevels, messageTypes, debugCallback);
		vk::InstanceCreateInfo instanceCreateInfo({}, &appInfo, enabled_layers, required_extensions, &enabled);
#else
		vk::InstanceCreateInfo instanceCreateInfo({}, &appInfo, enabled_layers, required_extensions);
#endif
		
		mVulkanInstance = vk::raii::Instance(mVulkanContext, instanceCreateInfo);

#ifdef ENABLE_VALIDATION_LAYERS
		mDebugMessenger = vk::raii::DebugUtilsMessengerEXT(mVulkanInstance, debugCreateInfo);
		
#endif
	}

	void VulkanBackend::CreateDebugMessenger()
	{

	}

	void VulkanBackend::PickPhysicalDevice()
	{
		auto devices = mVulkanInstance.enumeratePhysicalDevices();
		const auto devIter = std::ranges::find_if(
			devices,
			[&](const auto &device)
			{
				auto props = device.getProperties();
				const char *name = props.deviceName;
				auto queueFamilies = device.getQueueFamilyProperties();
				bool isSuitable = props.apiVersion >= vk::ApiVersion14;
				const auto qfpIter = std::ranges::find_if(queueFamilies, [](const vk::QueueFamilyProperties &qfp) { return (qfp.queueFlags & vk::QueueFlagBits::eGraphics) != (vk::QueueFlags)0; });
				isSuitable = isSuitable && (qfpIter != queueFamilies.end());

				auto extensions = device.enumerateDeviceExtensionProperties();
				bool found = true;
				for (const auto &extension : GetRequiredExtensions())
				{
					auto extensionIter = std::ranges::find_if(extensions, [extension](const auto &ext) { return strcmp(ext.extensionName, extension) == 0; });
					found = (extensionIter != extensions.end());
					if (!found)
					{
						LOG_ERROR("Required device extension {} not found for gpu: {}", extension, name);
						isSuitable = false;
					}
				}

				isSuitable = isSuitable && found;
				if (isSuitable)
				{
					mPhysicalDevice = device;
					
					LOG_INFO("Selected GPU: {}", name);
				}

				return isSuitable;
			});

		if (devIter == devices.end())
		{
			LOG_ERROR("Failed to find a suitable GPU!");
			ASSERT(false);
		}
	}

	void VulkanBackend::CreateImmediateCommandPool()
	{
		vk::CommandPoolCreateInfo pool_info(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, mQueueFamilies.GraphicsQueueIndex);
		mImmediateCommandPool = mLogicalDevice.createCommandPool(pool_info);
	}

	void VulkanBackend::CreateMemoryAllocator()
	{
		mMemoryAllocator = CreateScope<MemoryAllocator>(mLogicalDevice, mPhysicalDevice);
	}

	void VulkanBackend::CreateGPUResourceManager()
	{
		mGPUResourceManager = CreateScope<GPUResourceManager>();
	}

	void VulkanBackend::CreateDeviceInternal(uint32_t graphicsIndex, uint32_t presentIndex)
	{
		std::vector<uint32_t> families = {static_cast<uint32_t>(graphicsIndex), presentIndex};
		std::sort(families.begin(), families.end());
		families.erase(std::unique(families.begin(), families.end()), families.end());

		auto queue_priority = 1.0f;
		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
		for (auto f : families)
		{
			vk::DeviceQueueCreateInfo queueCreateInfo({}, f, 1, &queue_priority);
			queueCreateInfos.push_back(queueCreateInfo);
		}

		auto requiredDeviceExtensions = VulkanBackend::GetRequiredExtensions();
		auto featureChain = details::GetPhysicalDeviceFeaturesChain();

		vk::DeviceCreateInfo device_createInfo({}, queueCreateInfos, {}, requiredDeviceExtensions, nullptr, &featureChain.get<vk::PhysicalDeviceFeatures2>());

		mLogicalDevice = mPhysicalDevice.createDevice(device_createInfo);

		mQueueFamilies.GraphicsQueue = mLogicalDevice.getQueue(graphicsIndex, 0);
		mQueueFamilies.GraphicsQueueIndex = graphicsIndex;

		if (presentIndex == graphicsIndex)
		{
			mQueueFamilies.PresentQueueIndex = graphicsIndex;
			mQueueFamilies.PresentQueue = mQueueFamilies.GraphicsQueue;
		}
		else
		{
			mQueueFamilies.PresentQueueIndex = presentIndex;
			mQueueFamilies.PresentQueue = mLogicalDevice.getQueue(presentIndex, 0);
		}

		CreateImmediateCommandPool();
		CreateMemoryAllocator();
		CreateGPUResourceManager();
	}

	void VulkanBackend::EnsurePresentSupportForSurface(const vk::SurfaceKHR &surface)
	{
		if (!surface)
		{
			return;
		}

		if (mPhysicalDevice.getSurfaceSupportKHR(mQueueFamilies.GraphicsQueueIndex, surface))
		{
			mQueueFamilies.PresentQueueIndex = mQueueFamilies.GraphicsQueueIndex;
			mQueueFamilies.PresentQueue = mQueueFamilies.GraphicsQueue;
			return;
		}

		auto present_index = VulkanBackend::SelectQueueIndex(vk::QueueFlagBits::eGraphics , surface);
		if (present_index == ~0)
		{
			ASSERT(false, "No present queue found");
		}

		if (present_index != mQueueFamilies.PresentQueueIndex)
		{
			mLogicalDevice.waitIdle();
			mLogicalDevice = nullptr;
			CreateDeviceInternal(mQueueFamilies.GraphicsQueueIndex, present_index);
		}
	}

	void VulkanBackend::CreateLogicalDevice(const vk::SurfaceKHR &surface)
	{
		if (mLogicalDevice != VK_NULL_HANDLE)
		{
			return;
		}

		auto graphics_index = VulkanBackend::SelectQueueIndex(vk::QueueFlagBits::eGraphics );
		auto present_index = graphics_index;

		if (surface)
		{
			if (!mPhysicalDevice.getSurfaceSupportKHR(graphics_index, surface))
			{
				present_index = VulkanBackend::SelectQueueIndex(vk::QueueFlagBits::eGraphics , surface);
			}
		}

		CreateDeviceInternal(graphics_index, present_index);
	}

} // namespace BHive