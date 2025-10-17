#include "GraphicsContext.h"
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <vulkan/vulkan_to_string.hpp>
#include "VulkanUtils.h"
#include "VulkanSwapChain.h"
#include "RenderCommand.h"

#ifdef _DEBUG
	#define ENABLE_VALIDATION_LAYERS
#endif

#define MAX_FRAMES_IN_FLIGHT 2


namespace BHive
{
	static bool sFramebufferResized = false;

	static void framebufferResizeCallback(GLFWwindow *window, int width, int height)
	{
		sFramebufferResized = true;
	}

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
		ASSERT(!sInstance);
		sInstance = this;
		ASSERT(sInstance);

		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	}

	GraphicsContext::~GraphicsContext()
	{
		mDevice.waitIdle();

	}

	void GraphicsContext::Init()
	{
		CreateIntance();
		CreateDebugMessenger();
		PickPhysicalDevice();
		CreateSurface();
		CreateLogicalDevice();
		CreateSwapChain();
	}

	void GraphicsContext::SwapBuffers()
	{
		auto current_frame = mSwapChain->GetCurrentFrame();
		auto [result, imageIndex] = mSwapChain->AquireNextImage();
		mImageIndex = imageIndex;

		if (result == vk::Result::eErrorOutOfDateKHR)
		{
			RecreateSwapChain();
			return;
		}

		ASSERT(result == vk::Result::eSuccess || result == vk::Result::eSuboptimalKHR, "Failed to acquire swap chain image!");

		auto api = RenderCommand::GetAPI();
		auto& command_buffer = api->GetCurrentCommandBuffer();
		auto& buffers = api->GetAdditonalCommandBuffers();
		
		mSwapChain->ResetCommandBuffer(command_buffer);
		
		RenderCommand::BeginFrame();

		RenderCommand::EndFrame();

		std::vector<vk::CommandBuffer> buffers_to_submit = {command_buffer};
		while(!buffers.empty())
		{
			auto& buffer = buffers.front();
			buffers_to_submit.emplace_back(buffer);
			buffers.pop();
		}

		result = mSwapChain->SubmitCommandBuffers(buffers_to_submit, imageIndex);

		if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR || sFramebufferResized)
		{
			sFramebufferResized = false;
			RecreateSwapChain();
		}
		else if (result != vk::Result::eSuccess)
		{
			ASSERT(false, "Failed to present swap chain image!")
		}

		
	}

	constexpr uint32_t GraphicsContext::GetInstanceVersion() 
	{
		return vk::ApiVersion14;
	}

	void GraphicsContext::CreateIntance()
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

	void GraphicsContext::CreateDebugMessenger()
	{
#ifdef ENABLE_VALIDATION_LAYERS

		vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo(
			{}, vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
			vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance, &debugCallback);
		mDebugMessenger = vk::raii::DebugUtilsMessengerEXT(mVulkanInstance, debugCreateInfo);
#endif
	}

	void GraphicsContext::PickPhysicalDevice()
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

	void GraphicsContext::CreateLogicalDevice()
	{
		std::vector<vk::QueueFamilyProperties> queueFamilyProperties = mPhysicalDevice.getQueueFamilyProperties();
		for (uint32_t qfpIndex = 0; qfpIndex < static_cast<uint32_t>(queueFamilyProperties.size()); qfpIndex++)
		{
			if ((queueFamilyProperties[qfpIndex].queueFlags & vk::QueueFlagBits::eGraphics) && mPhysicalDevice.getSurfaceSupportKHR(qfpIndex, *mSurface))
			{
				mQueueFamilies.GraphicsQueueIndex = qfpIndex;
				break;
			}
		}

		if (mQueueFamilies.GraphicsQueueIndex == ~0)
		{
			LOG_ERROR("Failed to find a suitable queue family!");
			ASSERT(false);
		}

		auto queue_priority = 0.0f;
		auto requiredDeviceExtensions = GetRequiredExtensions();

		vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan11Features, vk::PhysicalDeviceVulkan13Features, vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT> featureChain;
		featureChain.assign<vk::PhysicalDeviceFeatures2>({}); // default initialize all features to false
		featureChain.get<vk::PhysicalDeviceVulkan11Features>().shaderDrawParameters = true;
		featureChain.get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering = true;
		featureChain.get<vk::PhysicalDeviceVulkan13Features>().synchronization2 = true;
		featureChain.get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().extendedDynamicState = true;

		vk::DeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.queueFamilyIndex = mQueueFamilies.GraphicsQueueIndex;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queue_priority;

		vk::DeviceCreateInfo device_createInfo{};
		device_createInfo.pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>();
		device_createInfo.queueCreateInfoCount = 1;
		device_createInfo.pQueueCreateInfos = &queueCreateInfo;
		device_createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredDeviceExtensions.size());
		device_createInfo.ppEnabledExtensionNames = requiredDeviceExtensions.data();

		mDevice = mPhysicalDevice.createDevice(device_createInfo);

		mQueueFamilies.GraphicsQueue = mDevice.getQueue(mQueueFamilies.GraphicsQueueIndex, 0);
	}
	
	std::vector<const char *> GraphicsContext::GetRequiredExtensions()
	{
		return {vk::KHRSwapchainExtensionName, vk::KHRSpirv14ExtensionName, vk::KHRSynchronization2ExtensionName, vk::KHRCreateRenderpass2ExtensionName};
	}

	void GraphicsContext::CreateSwapChain()
	{
		int w = 0, h = 0;
		glfwGetFramebufferSize(mWindowHandle, &w, &h);

		auto surfaceCapabilities =mPhysicalDevice.getSurfaceCapabilitiesKHR(*mSurface);
		auto formats = mPhysicalDevice.getSurfaceFormatsKHR(*mSurface);
		auto presentModes = mPhysicalDevice.getSurfacePresentModesKHR(*mSurface);

		VulkanSwapChainCreateInfo create_info{};
		create_info.Width = w;
		create_info.Height = h;
		create_info.Capabilities = surfaceCapabilities;
		create_info.Formats = formats;
		create_info.PresentModes = presentModes;
		mSwapChain = CreateRef<VulkanSwapChain>();
		mSwapChain->Init(mDevice, mSurface, create_info);
	}


	//void GraphicsContext::RecordCommandBuffer(uint32_t imageIndex)
	//{
	//	auto api = RenderCommand::GetAPI();
	//	auto current_frame = mSwapChain->GetCurrentFrame();
	//	auto &cmd = api->GetCurrentCommandBuffer();
	//	auto extent = mSwapChain->GetExtent();

	//	//cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, mGraphicsPipeline->GetPipeline());

	//	RenderCommand::SetViewport(0, 0, extent.width, extent.height);
	//	

	//	//cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, mPipelineLayout, 0, *mDescriptorSets[current_frame], nullptr);

	//	//RenderCommand::DrawElements(EDrawMode::Triangles, *mVertexArray);
	//}

	void GraphicsContext::RecreateSwapChain()
	{
		int width = 0, height = 0;
		glfwGetFramebufferSize(mWindowHandle, &width, &height);
		while (width == 0 || height == 0)
		{
			glfwGetFramebufferSize(mWindowHandle, &width, &height);
			glfwWaitEvents();
		}

		mDevice.waitIdle();
		CreateSwapChain();
	}

	

	void GraphicsContext::CreateSurface()
	{
		VkSurfaceKHR _surface;

		if (glfwCreateWindowSurface(*mVulkanInstance, mWindowHandle, nullptr, &_surface) != VK_SUCCESS)
		{
			LOG_ERROR("Failed to create window surface!");
			ASSERT(false);
		}
		mSurface = vk::raii::SurfaceKHR(mVulkanInstance, _surface);
	}

} // namespace BHive