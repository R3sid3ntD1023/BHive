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

namespace BHive
{
	static const std::vector<const char *> s_validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

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

#ifdef VULKAN_ERRORS_WITH_ASSERT
		if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
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
			features.setFillModeNonSolid(true).setWideLines(true).setMultiDrawIndirect(true)
				.setDrawIndirectFirstInstance(true);

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
				.setDescriptorBindingUniformBufferUpdateAfterBind(true)
				.setDescriptorBindingStorageImageUpdateAfterBind(true);
			featureChain.get<vk::PhysicalDeviceVulkan13Features>().setDynamicRendering(true).setSynchronization2(true).setDescriptorBindingInlineUniformBlockUpdateAfterBind(true).setMaintenance4(true);
			featureChain.get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().setExtendedDynamicState(true);
			featureChain.get<vk::PhysicalDeviceVertexInputDynamicStateFeaturesEXT>().setVertexInputDynamicState(true);
			featureChain.get<vk::PhysicalDeviceVertexAttributeDivisorFeaturesEXT>().setVertexAttributeInstanceRateZeroDivisor(true);
			featureChain.get<vk::PhysicalDeviceRobustness2FeaturesKHR>().setNullDescriptor(true).setRobustImageAccess2(true);

			return featureChain;
		}
	} // namespace details

	void VulkanBackend::Init(GLFWwindow* window)
	{
		VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

		CreateIntance();
		CreateDebugMessenger();
		PickPhysicalDevice();
		CreateWindowSurface(window);
		CreateLogicalDevice(mSurface);
		CreateSwapChain(window);
		CreateCommandBuffers();
		CreateSyncObjects();

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
		if (*mDevice)
			mDevice.waitIdle();
	}

	vk::Result VulkanBackend::Present()
	{
		auto api = RenderCommand::GetGraphicsAPI<VulkanRendererAPI>();
		return api->RenderFrame(mSwapChain.get());
	}

	void VulkanBackend::RequestSwapChainRecreate(uint32_t w, uint32_t h)
	{
		mDevice.waitIdle();

		LOG_TRACE("recreating swap chain... with size[{}x{}]", w, h);

		mSwapChain->Recreate(mDevice,w, h);
		RecreateFrameResources();
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

	vk::Semaphore VulkanBackend::GetRenderFinishedSemaphore(uint32_t imageIndex)
	{
		return Get().mRenderFinishedSemaphores.at(imageIndex);
	}

	vk::Semaphore VulkanBackend::GetImageAvailableSemaphore(uint32_t frame)
	{
		return Get().mPresentSemaphores.at(frame);
	}

	vk::Fence VulkanBackend::GetInFlightFence(uint32_t frame)
	{
		return Get().mInFlightFences.at(frame);
	}

	void VulkanBackend::CreateIntance()
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

		vk::ValidationFeatureEnableEXT enabled_features[] =
			{
				//vk::ValidationFeatureEnableEXT::eBestPractices, 
				vk::ValidationFeatureEnableEXT::eSynchronizationValidation, 
				//vk::ValidationFeatureEnableEXT::eDebugPrintf,
				//vk::ValidationFeatureEnableEXT::eGpuAssisted
			};

		vk::ValidationFeaturesEXT enabled(enabled_features);
	
		auto loglevels = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
						 vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo;
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

	void VulkanBackend::CreateWindowSurface(GLFWwindow *window)
	{
		VkSurfaceKHR _surface;
		auto &instance = VulkanBackend::GetInstance();
		if (glfwCreateWindowSurface(*instance, window, nullptr, &_surface) != VK_SUCCESS)
		{
			LOG_ERROR("Failed to create window surface!");
			ASSERT(false);
		}

		mSurface = vk::raii::SurfaceKHR(instance, _surface);

		VulkanBackend::Get().CreateLogicalDevice(mSurface);
		VulkanBackend::Get().EnsurePresentSupportForSurface(*mSurface);
	}

	void VulkanBackend::CreateSwapChain(GLFWwindow *window)
	{
		int w = 0, h = 0;
		glfwGetFramebufferSize(window, &w, &h);

		while (w == 0 || h == 0)
		{
			glfwWaitEvents();
			glfwGetFramebufferSize(window, &w, &h);
		}

		mSwapChain = CreateScope<VulkanSwapChain>(mDevice, mSurface, uint32_t(w), uint32_t(h));
	}

	void VulkanBackend::CreateSyncObjects()
	{
		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			mPresentSemaphores.emplace_back(mDevice, vk::SemaphoreCreateInfo());
			mInFlightFences.emplace_back(mDevice, vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
		}
	}

	void VulkanBackend::CreatePerImageSync(uint32_t imgCount)
	{
		mRenderFinishedSemaphores.clear();

		for (uint32_t i = 0; i < imgCount; i++)
		{
			mRenderFinishedSemaphores.emplace_back(mDevice, vk::SemaphoreCreateInfo());
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

		CreateImmediateCommandPool();
		CreateMemoryAllocator();
		CreateGPUResourceManager();
	}

	void VulkanBackend::RecreateFrameResources()
	{
		/*mCommandBuffers = nullptr;
		mCommandPool = nullptr;

		CreateCommandBuffers();*/
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
			mDevice.waitIdle();
			mDevice = nullptr;
			CreateDeviceInternal(mQueueFamilies.GraphicsQueueIndex, present_index);
		}
	}

	void VulkanBackend::CreateLogicalDevice(const vk::SurfaceKHR &surface)
	{
		if (mDevice != VK_NULL_HANDLE)
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