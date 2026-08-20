#include "VulkanBackend.h"
#include "core/debug/CrashHandler.h"
#include "gfx/RenderCommand.h"
#include "VulkanRendererAPI.h"
#include "VulkanUtils.h"
#include <GLFW/glfw3.h>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

#ifdef _DEBUG
	#define VALIDATION_LAYERS_ENABLED
	#ifdef VALIDATION_LAYERS_ENABLED
		#define ENABLE_VALIDATION_LAYERS
	#endif
#endif

#define VULKAN_ERRORS_WITH_ASSERT 1

namespace BHive
{
	static const std::vector<const char *> s_validationLayers = {"VK_LAYER_KHRONOS_validation"};

	static VKAPI_ATTR vk::Bool32 VKAPI_CALL
	debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity, vk::DebugUtilsMessageTypeFlagsEXT messageType, const vk::DebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
	{
		auto message_type_string = vk::to_string(messageType);
		auto registry = reinterpret_cast<VulkanBackend::DebugNameRegistry *>(pUserData);

		if (messageSeverity >= vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning)
		{
			for (uint32_t i = 0; i < pCallbackData->objectCount; i++)
			{
				const auto &obj = pCallbackData->pObjects[i];

				auto name = registry->GetName(obj.objectType, obj.objectHandle);

				printf("Vulkan Validation: %s (type=%d, handle=%llu)\n", name.c_str(), obj.objectType, (unsigned long long)obj.objectHandle);
			}
		}

		switch (messageSeverity)
		{
		case vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose:
			LOG_TRACE("{} : {}", message_type_string, pCallbackData->pMessage);
			break;
		case vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo:
			LOG_INFO("{} : {}", message_type_string, pCallbackData->pMessage);
			break;
		case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:
			LOG_WARN("{} : {}", message_type_string, pCallbackData->pMessage);
			break;
		case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:
			LOG_ERROR("{} : {}", message_type_string, pCallbackData->pMessage);
			break;
		default:
			break;
		}

#if VULKAN_ERRORS_WITH_ASSERT
		if (messageSeverity >= vk::DebugUtilsMessageSeverityFlagBitsEXT::eError)
		{
			ASSERT(false);
		}
#endif

		return false;
	}

	namespace details
	{
		auto GetPhysicalDeviceFeaturesChain()
		{

			vk::PhysicalDeviceFeatures features{};
			features.setFillModeNonSolid(true).setWideLines(true).setMultiDrawIndirect(true).setDrawIndirectFirstInstance(true);

			vk::StructureChain<
				vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan11Features, vk::PhysicalDeviceVulkan12Features, vk::PhysicalDeviceVulkan13Features,
				vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT, vk::PhysicalDeviceVertexInputDynamicStateFeaturesEXT, vk::PhysicalDeviceVertexAttributeDivisorFeaturesEXT,
				vk::PhysicalDeviceRobustness2FeaturesKHR, vk::PhysicalDeviceShaderObjectFeaturesEXT>
				featureChain;
			featureChain.assign<vk::PhysicalDeviceFeatures2>({}); // default initialize all features to false

			featureChain.get<vk::PhysicalDeviceFeatures2>().setFeatures(features);
			featureChain.get<vk::PhysicalDeviceVulkan11Features>().setShaderDrawParameters(true);
			featureChain.get<vk::PhysicalDeviceVulkan12Features>()
				.setDescriptorBindingSampledImageUpdateAfterBind(true)
				.setDescriptorBindingPartiallyBound(true)
				.setDescriptorBindingUpdateUnusedWhilePending(true)
				.setDescriptorBindingStorageBufferUpdateAfterBind(true)
				.setDescriptorBindingUniformBufferUpdateAfterBind(true)
				.setDescriptorBindingStorageImageUpdateAfterBind(true);
			featureChain.get<vk::PhysicalDeviceVulkan13Features>().setDynamicRendering(true).setSynchronization2(true).setDescriptorBindingInlineUniformBlockUpdateAfterBind(true).setMaintenance4(
				true);
			featureChain.get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().setExtendedDynamicState(true);
			featureChain.get<vk::PhysicalDeviceVertexInputDynamicStateFeaturesEXT>().setVertexInputDynamicState(true);
			featureChain.get<vk::PhysicalDeviceVertexAttributeDivisorFeaturesEXT>().setVertexAttributeInstanceRateZeroDivisor(true);
			featureChain.get<vk::PhysicalDeviceRobustness2FeaturesKHR>().setNullDescriptor(true).setRobustImageAccess2(true);
			featureChain.get<vk::PhysicalDeviceShaderObjectFeaturesEXT>().setShaderObject(true);

			return featureChain;
		}
	} // namespace details

	void VulkanBackend::Init()
	{
		VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

		CreateInstance();
		CreateDebugMessenger();
		PickPhysicalDevice();
		CreateLogicalDevice();
		CreateCommandBuffers();
		CreateDescriptorPool();
		CreateImmediateCommandPool();
		CreateMemoryAllocator();
		CreateGPUResourceManager();

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
		mDevice.waitIdle();

		mDescriptorPool.clear();

		mCommandBuffers.clear();

		mCommandPool.clear();
		mImmediateCommandPool.clear();

		mDebugMessenger.clear();

		// mDevice.clear();

		// mInstance.clear();

		// mGPUResourceManager.reset();

		// mMemoryAllocator.reset();
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
			vk::EXTVertexAttributeDivisorExtensionName,
			vk::EXTShaderObjectExtensionName};
	}

	uint32_t VulkanBackend::SelectQueueIndex(vk::QueueFlags queue_type, vk::SurfaceKHR surface)
	{
		std::vector<vk::QueueFamilyProperties> queueFamilyProperties = mPhysicalDevice.getQueueFamilyProperties();
		for (uint32_t qfpIndex = 0; qfpIndex < static_cast<uint32_t>(queueFamilyProperties.size()); qfpIndex++)
		{
			if ((queueFamilyProperties[qfpIndex].queueFlags & queue_type) && mPhysicalDevice.getSurfaceSupportKHR(qfpIndex, surface))
			{
				return qfpIndex;
			}
		}

		return ~0u;
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

		return ~0u;
	}

	void VulkanBackend::CreateInstance()
	{
		constexpr auto appInfo = vk::ApplicationInfo{"BHive", 1, "BHiveEngine", 1, MINIMUM_VULKAN_API_VERSION};

		uint32_t glfwExtensionCount = 0;
		const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		auto extensionProperties = mContext.enumerateInstanceExtensionProperties();
		auto layerProperties = mContext.enumerateInstanceLayerProperties();

		std::vector required_extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
		std::vector<const char *> enabled_layers;

#ifdef ENABLE_VALIDATION_LAYERS
		enabled_layers.assign(s_validationLayers.begin(), s_validationLayers.end());
		required_extensions.push_back(vk::EXTDebugUtilsExtensionName);
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
			if (std::ranges::none_of(extensionProperties, [glfwExtensions, i](const vk::ExtensionProperties &prop) { return strcmp(prop.extensionName, glfwExtensions[i]) == 0; }))
			{
				LOG_ERROR("Missing required Vulkan extension: {}", glfwExtensions[i]);
				ASSERT(false);
			}
		}

#ifdef ENABLE_VALIDATION_LAYERS

		vk::ValidationFeatureEnableEXT enabled_features[] = {
			vk::ValidationFeatureEnableEXT::eSynchronizationValidation,
			// vk::ValidationFeatureEnableEXT::eBestPractices,
			//  vk::ValidationFeatureEnableEXT::eDebugPrintf,
			// vk::ValidationFeatureEnableEXT::eGpuAssisted
		};

		vk::ValidationFeaturesEXT enabled(enabled_features);

		auto loglevels = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
						 | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo;
		auto messageTypes = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;

		vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo({}, loglevels, messageTypes, debugCallback, &mDebugNames);
		vk::InstanceCreateInfo instanceCreateInfo({}, &appInfo, enabled_layers, required_extensions, &enabled);
#else
		vk::InstanceCreateInfo instanceCreateInfo({}, &appInfo, enabled_layers, required_extensions);
#endif

		mInstance = vk::raii::Instance(mContext, instanceCreateInfo);
		VULKAN_HPP_DEFAULT_DISPATCHER.init((vk::Instance)mInstance);

#ifdef ENABLE_VALIDATION_LAYERS
		mDebugMessenger = vk::raii::DebugUtilsMessengerEXT(mInstance, debugCreateInfo);

#endif
	}

	void VulkanBackend::CreateDebugMessenger()
	{
	}

	void VulkanBackend::PickPhysicalDevice()
	{
		auto devices = mInstance.enumeratePhysicalDevices();
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

	void VulkanBackend::CreateCommandBuffers()
	{
		auto graphics_queue_index = mQueueFamilies.GraphicsQueueIndex;

		vk::CommandPoolCreateInfo pool_info(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, graphics_queue_index);
		mCommandPool = vk::raii::CommandPool(mDevice, pool_info);

		vk::CommandBufferAllocateInfo alloc_info(mCommandPool, vk::CommandBufferLevel::ePrimary, MAX_FRAMES_IN_FLIGHT);
		mCommandBuffers = vk::raii::CommandBuffers(mDevice, alloc_info);
	}

	void VulkanBackend::CreateImmediateCommandPool()
	{
		vk::CommandPoolCreateInfo pool_info(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, mQueueFamilies.GraphicsQueueIndex);
		mImmediateCommandPool = mDevice.createCommandPool(pool_info);
	}

	void VulkanBackend::CreateDescriptorPool()
	{
		static vk::DescriptorPoolCreateFlags poolFlags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;

		// --- Material pool (set = 1, cached per material) ---
		std::vector<vk::DescriptorPoolSize> materialSizes
			= {{vk::DescriptorType::eCombinedImageSampler, 4096},
			   {vk::DescriptorType::eSampledImage, 4096},
			   {vk::DescriptorType::eStorageImage, 2048},
			   {vk::DescriptorType::eUniformBuffer, 1024},
			   {vk::DescriptorType::eStorageBuffer, 1024}};

		vk::DescriptorPoolCreateInfo matInfo{poolFlags, 4096, materialSizes};

		mDescriptorPool = mDevice.createDescriptorPool(matInfo);
	}

	void VulkanBackend::CreateMemoryAllocator()
	{
		mMemoryAllocator = CreateScope<MemoryAllocator>(mDevice, mPhysicalDevice);
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

		mDevice = mPhysicalDevice.createDevice(device_createInfo);
		VULKAN_HPP_DEFAULT_DISPATCHER.init((vk::Device)mDevice);

		mQueueFamilies.GraphicsQueue = mDevice.getQueue(graphicsIndex, 0);
		mQueueFamilies.GraphicsQueueIndex = graphicsIndex;

		if (presentIndex == graphicsIndex)
		{
			mQueueFamilies.PresentQueueIndex = graphicsIndex;
			mQueueFamilies.PresentQueue = mQueueFamilies.GraphicsQueue;
		}
		else
		{
			mQueueFamilies.PresentQueueIndex = presentIndex;
			mQueueFamilies.PresentQueue = mDevice.getQueue(presentIndex, 0);
		}
	}

	bool VulkanBackend::EnsurePresentSupportForSurface(vk::SurfaceKHR surface)
	{
		if (!surface)
		{
			return false;
		}

		if (mPhysicalDevice.getSurfaceSupportKHR(mQueueFamilies.GraphicsQueueIndex, surface))
		{
			mQueueFamilies.PresentQueueIndex = mQueueFamilies.GraphicsQueueIndex;
			mQueueFamilies.PresentQueue = mQueueFamilies.GraphicsQueue;
			return true;
		}

		LOG_ERROR("No present queue supports this surface!")
		return false;
	}

	void VulkanBackend::CreateLogicalDevice()
	{
		if (mDevice != VK_NULL_HANDLE)
		{
			return;
		}

		auto graphics_index = VulkanBackend::SelectQueueIndex(vk::QueueFlagBits::eGraphics);
		auto present_index = graphics_index;

		CreateDeviceInternal(graphics_index, present_index);
	}

} // namespace BHive