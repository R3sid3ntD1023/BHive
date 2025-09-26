#include "GraphicsContext.h"
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <vulkan/vulkan_to_string.hpp>

namespace BHive
{
	constexpr bool enabledValidateLayers = true;
	const std::vector<const char *> validationLayers = {
		"VK_LAYER_KHRONOS_validation",
	};
	std::vector<const char *> devExtensions = {vk::KHRSwapchainExtensionName, vk::KHRSpirv14ExtensionName, vk::KHRSynchronization2ExtensionName, vk::KHRCreateRenderpass2ExtensionName};
	constexpr int MAX_FRAMES_IN_FLIGHT = 2;

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

	GraphicsContext::~GraphicsContext()
	{
		mDevice.waitIdle();
		CleanupSwapChain();
	}

	void GraphicsContext::Init()
	{
		CreateIntance();
		CreateDebugMessenger();
		CreateSurface();
		PickPhysicalDevice();
		CreateLogicalDevice();
		CreateSwapChain();
		CreateImageViews();
		CreateGraphicsPipeline();
		CreateCommandPool();
		CreateCommandBuffers();
		CreateSyncObjects();
	}

	void GraphicsContext::SwapBuffers()
	{
		while (vk::Result::eTimeout == mDevice.waitForFences(*mInFlightFences[mCurrentFrame], vk::True, UINT64_MAX))
			;
		auto [result, imageIndex] = mSwapChain.acquireNextImage(UINT64_MAX, *mImageAvailableSemaphores[mCurrentFrame], nullptr);
		mDevice.resetFences(*mInFlightFences[mCurrentFrame]);

		mCommandBuffers[mCurrentFrame].reset();
		RecordCommandBuffer(mCommandBuffers[mCurrentFrame], imageIndex);

		vk::PipelineStageFlags waitStages = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		const vk::SubmitInfo submitInfo(*mImageAvailableSemaphores[mCurrentFrame], waitStages, *mCommandBuffers[mCurrentFrame], *mRenderFinishedSemaphores[mCurrentFrame]);

		mGraphicsQueue.submit(submitInfo, *mInFlightFences[mCurrentFrame]);

		const vk::PresentInfoKHR presentInfo(*mRenderFinishedSemaphores[mCurrentFrame], *mSwapChain, imageIndex);
		auto presResult = mPresentQueue.presentKHR(presentInfo);

		mCurrentFrame = (mCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	void GraphicsContext::CreateIntance()
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
		vk::InstanceCreateInfo instanceCreateInfo({}, &appInfo, requiredLayers, extensions, *mDebugMessenger);

		mVulkanInstance = vk::raii::Instance(mVulkanContext, instanceCreateInfo);
	}

	void GraphicsContext::CreateDebugMessenger()
	{
#if defined(_DEBUG)

		if (enabledValidateLayers)
		{

			vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo(
				{}, vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
				vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance, &debugCallback);
			mDebugMessenger = vk::raii::DebugUtilsMessengerEXT(mVulkanInstance, debugCreateInfo);
		}
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
				for (const auto &extension : devExtensions)
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

	void GraphicsContext::CreateSwapChain()
	{
		auto surfaceCapabilities = mPhysicalDevice.getSurfaceCapabilitiesKHR(*mSurface);
		mSwapChainImageFormat = ChooseSwapSurfaceFormat(mPhysicalDevice.getSurfaceFormatsKHR(*mSurface));
		mSwapChainExtent = ChooseSwapExtent(surfaceCapabilities);
		auto minImageCount = ChooseMinImageCount(surfaceCapabilities);

		auto present_mode = ChooseSwapPresentMode(mPhysicalDevice.getSurfacePresentModesKHR(*mSurface));
		vk::SwapchainCreateInfoKHR create_info(
			{}, *mSurface, minImageCount, mSwapChainImageFormat.format, mSwapChainImageFormat.colorSpace, mSwapChainExtent, 1, vk::ImageUsageFlagBits::eColorAttachment, vk::SharingMode::eExclusive,
			{}, surfaceCapabilities.currentTransform, vk::CompositeAlphaFlagBitsKHR::eOpaque, present_mode, true, nullptr);

		mSwapChain = vk::raii::SwapchainKHR(mDevice, create_info);
		mSwapChainImages = mSwapChain.getImages();
	}

	void GraphicsContext::CreateImageViews()
	{
		mSwapChainImageViews.clear();

		vk::ImageViewCreateInfo view_info({}, {}, vk::ImageViewType::e2D, mSwapChainImageFormat.format, {}, {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});

		for (auto &image : mSwapChainImages)
		{
			view_info.image = image;
			mSwapChainImageViews.emplace_back(mDevice, view_info);
		}
	}

	void GraphicsContext::CreateGraphicsPipeline()
	{
		std::vector dynamicStates = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
		vk::PipelineDynamicStateCreateInfo dynamicStateInfo({}, dynamicStates);
		vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo({}, vk::PrimitiveTopology::eTriangleList, VK_FALSE);
		vk::Viewport viewport(0.0f, 0.0f, static_cast<float>(mSwapChainExtent.width), static_cast<float>(mSwapChainExtent.height), 0.0f, 1.0f);
		vk::PipelineViewportStateCreateInfo viewportStateInfo({}, 1, &viewport, 1, &(vk::Rect2D{{0, 0}, mSwapChainExtent}));
		vk::PipelineRasterizationStateCreateInfo rasterizerInfo(
			{}, VK_FALSE, VK_FALSE, vk::PolygonMode::eFill, vk::CullModeFlagBits::eBack, vk::FrontFace::eClockwise, VK_FALSE, 0.0f, 0.0f, 0.0f, 1.0f);

		vk::PipelineColorBlendAttachmentState colorBlendAttachment(VK_FALSE);
		vk::PipelineVertexInputStateCreateInfo vertexInputInfo({}, 0, nullptr, 0, nullptr);
		vk::PipelineRenderingCreateInfo renderingCreateInfo({}, 1, &mSwapChainImageFormat.format);
		vk::GraphicsPipelineCreateInfo pipeline_info({}, 0, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, {}, {}, 0, nullptr, {}, -1);
	}

	void GraphicsContext::CreateCommandPool()
	{
		vk::CommandPoolCreateInfo pool_info(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, mGraphicsFamilyIndex);
		mCommandPool = vk::raii::CommandPool(mDevice, pool_info);
	}

	void GraphicsContext::CreateCommandBuffers()
	{
		mCommandBuffers.clear();

		vk::CommandBufferAllocateInfo alloc_info(*mCommandPool, vk::CommandBufferLevel::ePrimary, MAX_FRAMES_IN_FLIGHT);
		mCommandBuffers = vk::raii::CommandBuffers(mDevice, alloc_info);
	}

	void GraphicsContext::RecordCommandBuffer(vk::raii::CommandBuffer &cmd, uint32_t imageIndex)
	{
		cmd.begin({});

		vk::ClearValue clearColor = vk::ClearColorValue(0, 0, 0, 1);
		vk::RenderingAttachmentInfo attachmentInfo(
			mSwapChainImageViews[imageIndex], vk::ImageLayout::eColorAttachmentOptimal, vk::ResolveModeFlagBits::eNone, {}, {}, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
			clearColor);

		vk::RenderingInfo renderingInfo({}, vk::Rect2D({0, 0}, mSwapChainExtent), 1, 1, attachmentInfo);

		cmd.beginRendering(renderingInfo);
		cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, *mGraphicsPipeline);
		cmd.setViewport(0, vk::Viewport(0.0f, 0.0f, static_cast<float>(mSwapChainExtent.width), static_cast<float>(mSwapChainExtent.height), 0.0f, 1.0f));
		cmd.setScissor(0, vk::Rect2D({0, 0}, mSwapChainExtent));
		cmd.draw(3, 1, 0, 0);
		cmd.endRendering();

		transition_image_layout(
			imageIndex, vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal, {}, vk::AccessFlagBits2::eColorAttachmentWrite, vk::PipelineStageFlagBits2::eTopOfPipe,
			vk::PipelineStageFlagBits2::eColorAttachmentOutput);
		cmd.end();
	}

	void GraphicsContext::CreateSyncObjects()
	{
		mImageAvailableSemaphores.clear();
		mRenderFinishedSemaphores.clear();

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			mImageAvailableSemaphores.emplace_back(mDevice, vk::SemaphoreCreateInfo{});
			mRenderFinishedSemaphores.emplace_back(mDevice, vk::SemaphoreCreateInfo{});
			mInFlightFences.emplace_back(mDevice, vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
		}
	}

	void GraphicsContext::RecreateSwapChain()
	{
		mDevice.waitIdle();

		CleanupSwapChain();

		CreateSwapChain();
		CreateImageViews();
	}

	void GraphicsContext::CleanupSwapChain()
	{
		mSwapChainImages.clear();
		mSwapChain = nullptr;
	}

	void GraphicsContext::transition_image_layout(
		uint32_t imageIndex, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::AccessFlags2 srcAccessMask, vk::AccessFlags2 dstAccessMask, vk::PipelineStageFlags2 srcStageMask,
		vk::PipelineStageFlags2 dstStageMask)
	{
		vk::ImageMemoryBarrier2 barrier(
			srcStageMask, srcAccessMask, dstStageMask, dstAccessMask, oldLayout, newLayout, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, mSwapChainImages[imageIndex],
			{vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});
		vk::DependencyInfo depInfo(vk::DependencyFlags(), {}, {}, barrier);
		mCommandBuffers[mCurrentFrame].pipelineBarrier2(depInfo);
	};

	uint32_t GraphicsContext::FindQueueFamilies(vk::PhysicalDevice device)
	{
		std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();
		auto graphicsQueueFamilyProperty =
			std::find_if(queueFamilies.begin(), queueFamilies.end(), [](const vk::QueueFamilyProperties &qfp) { return (qfp.queueFlags & vk::QueueFlagBits::eGraphics); });

		return static_cast<uint32_t>(std::distance(queueFamilies.begin(), graphicsQueueFamilyProperty));
	}

	void GraphicsContext::CreateLogicalDevice()
	{
		std::vector<vk::QueueFamilyProperties> queueFamilyProperties = mPhysicalDevice.getQueueFamilyProperties();
		auto numQueueFamilies = static_cast<uint32_t>(queueFamilyProperties.size());
		auto graphics_index = FindQueueFamilies(*mPhysicalDevice);
		auto present_index = mPhysicalDevice.getSurfaceSupportKHR(graphics_index, *mSurface) ? graphics_index : numQueueFamilies;

		if (present_index == numQueueFamilies)
		{
			for (uint32_t i = 0; i < numQueueFamilies; i++)
			{
				if ((queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics) && mPhysicalDevice.getSurfaceSupportKHR(i, *mSurface))
				{
					graphics_index = i;
					present_index = graphics_index;
					break;
				}
			}

			if (present_index == numQueueFamilies)
			{
				for (uint32_t i = 0; i < numQueueFamilies; i++)
				{
					if (mPhysicalDevice.getSurfaceSupportKHR(i, *mSurface))
					{
						present_index = i;
						break;
					}
				}
			}
		}

		if ((graphics_index == numQueueFamilies) || (present_index == numQueueFamilies))
		{
			LOG_ERROR("Failed to find a suitable GPU queue family!");
			ASSERT(false);
		}

		auto queue_priority = 0.0f;

		vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan13Features, vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT> featureChain;
		featureChain.assign<vk::PhysicalDeviceFeatures2>({}); // default initialize all features to false
		featureChain.get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering = true;
		featureChain.get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().extendedDynamicState = true;

		vk::DeviceQueueCreateInfo queueCreateInfo({}, graphics_index, 1, &queue_priority);

		vk::DeviceCreateInfo createInfo({}, queueCreateInfo, {}, devExtensions, &featureChain.get<vk::PhysicalDeviceFeatures2>().features);
		mDevice = vk::raii::Device(mPhysicalDevice, createInfo);

		mGraphicsQueue = mDevice.getQueue(graphics_index, 0);
		mPresentQueue = mDevice.getQueue(present_index, 0);

		mGraphicsFamilyIndex = graphics_index;
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

	vk::SurfaceFormatKHR GraphicsContext::ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats)
	{
		for (const auto &availableFormat : availableFormats)
		{
			if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
			{
				return availableFormat;
			}
		}

		return availableFormats[0];
	}

	vk::PresentModeKHR GraphicsContext::ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes)
	{
		for (const auto &mode : availablePresentModes)
		{
			if (mode == vk::PresentModeKHR::eMailbox)
			{
				return mode;
			}
		}
		return vk::PresentModeKHR::eFifo;
	}

	vk::Extent2D GraphicsContext::ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities)
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			return capabilities.currentExtent;
		}
		else
		{
			int width, height;
			glfwGetFramebufferSize(mWindowHandle, &width, &height);
			vk::Extent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
			actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
			return actualExtent;
		}
	}

	uint32_t GraphicsContext::ChooseMinImageCount(vk::SurfaceCapabilitiesKHR capabilities)
	{
		auto minImageCount = std::max(3u, capabilities.minImageCount);
		if (capabilities.maxImageCount > 0 && minImageCount > capabilities.maxImageCount)
		{
			minImageCount = capabilities.maxImageCount;
		}

		return minImageCount;
	}

} // namespace BHive