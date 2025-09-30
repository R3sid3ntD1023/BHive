#include "GraphicsContext.h"
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <vulkan/vulkan_to_string.hpp>
#include "ShaderManager.h"
#include "Shader.h"

namespace BHive
{
#ifdef _DEBUG
	#define ENABLE_VALIDATION_LAYERS
#endif

	const std::vector<const char *> validationLayers = {
		"VK_LAYER_KHRONOS_validation",
	};
	std::vector<const char *> requiredDeviceExtensions = {vk::KHRSwapchainExtensionName, vk::KHRSpirv14ExtensionName, vk::KHRSynchronization2ExtensionName, vk::KHRCreateRenderpass2ExtensionName};
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

	static bool sFramebufferResized = false;

	static void framebufferResizeCallback(GLFWwindow *window, int width, int height)
	{
		sFramebufferResized = true;
	}

	GraphicsContext::GraphicsContext(GLFWwindow *window)
		: mWindowHandle(window)
	{
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
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

		while (vk::Result::eTimeout == mDevice.waitForFences(*mInFlightFences[mCurrentFrame], VK_TRUE, UINT64_MAX))
			;

		auto [result, imageIndex] = mSwapChain.acquireNextImage(UINT64_MAX, mPresetCompleteSemaphores[mCurrentFrame], nullptr);
		if (result == vk::Result::eErrorOutOfDateKHR)
		{
			RecreateSwapChain();
			return;
		}

		if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
		{
			LOG_ERROR("Failed to acquire swap chain image!");
			ASSERT(false);
		}

		mDevice.resetFences(*mInFlightFences[mCurrentFrame]);
		mCommandBuffers[mCurrentFrame].reset();

		RecordCommandBuffer(imageIndex);

		vk::PipelineStageFlags waitStages(vk::PipelineStageFlagBits::eColorAttachmentOutput);

		const vk::SubmitInfo submitInfo(*mPresetCompleteSemaphores[mCurrentFrame], waitStages, *mCommandBuffers[mCurrentFrame], *mRenderFinishedSemaphores[imageIndex]);
		mQueue.submit(submitInfo, *mInFlightFences[mCurrentFrame]);

		const vk::PresentInfoKHR presentInfoKHR(*mRenderFinishedSemaphores[imageIndex], *mSwapChain, imageIndex, result);

		result = mQueue.presentKHR(presentInfoKHR);
		if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR || sFramebufferResized)
		{
			sFramebufferResized = false;
			RecreateSwapChain();
		}
		else if (result != vk::Result::eSuccess)
		{
			LOG_ERROR("Failed to present swap chain image!");
			ASSERT(false);
		}

		mCurrentSemasphore = (mCurrentSemasphore + 1) % mPresetCompleteSemaphores.size();
		mCurrentFrame = (mCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	void GraphicsContext::CreateIntance()
	{
		constexpr auto appInfo = vk::ApplicationInfo{"BHive", 1, "No Engine", 1, vk::ApiVersion14};

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
				for (const auto &extension : requiredDeviceExtensions)
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
		mSwapChainExtent = ChooseSwapExtent(surfaceCapabilities);
		mSwapChainImageFormat = ChooseSwapSurfaceFormat(mPhysicalDevice.getSurfaceFormatsKHR(*mSurface));

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
		ASSERT(mSwapChainImageViews.empty())

		vk::ImageViewCreateInfo view_info({}, {}, vk::ImageViewType::e2D, mSwapChainImageFormat.format, {}, {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});

		for (auto &image : mSwapChainImages)
		{
			view_info.image = image;
			mSwapChainImageViews.emplace_back(mDevice, view_info);
		}
	}

	void GraphicsContext::CreateGraphicsPipeline()
	{

		vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
		vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
		inputAssemblyInfo.topology = vk::PrimitiveTopology::eTriangleList;

		vk::PipelineViewportStateCreateInfo viewportStateInfo{};
		viewportStateInfo.viewportCount = 1;
		viewportStateInfo.scissorCount = 1;

		vk::PipelineRasterizationStateCreateInfo rasterizerInfo{};
		rasterizerInfo.depthClampEnable = VK_FALSE;
		rasterizerInfo.rasterizerDiscardEnable = VK_FALSE;
		rasterizerInfo.polygonMode = vk::PolygonMode::eFill;
		rasterizerInfo.cullMode = vk::CullModeFlagBits::eBack;
		rasterizerInfo.frontFace = vk::FrontFace::eClockwise;
		rasterizerInfo.depthBiasEnable = VK_FALSE;
		rasterizerInfo.depthBiasSlopeFactor = 1.0f;
		rasterizerInfo.lineWidth = 1.0f;

		vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.blendEnable = VK_FALSE;
		colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;

		vk::PipelineColorBlendStateCreateInfo blend_state_create_info{};
		blend_state_create_info.logicOpEnable = VK_FALSE;
		blend_state_create_info.logicOp = vk::LogicOp::eCopy;
		blend_state_create_info.attachmentCount = 1;
		blend_state_create_info.pAttachments = &colorBlendAttachment;

		vk::PipelineMultisampleStateCreateInfo multismapling{};
		multismapling.rasterizationSamples = vk::SampleCountFlagBits::e1;
		multismapling.sampleShadingEnable = VK_FALSE;

		std::vector dynamicStates = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};

		vk::PipelineDynamicStateCreateInfo dynamicStateInfo{};
		dynamicStateInfo.pDynamicStates = dynamicStates.data();
		dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());

		vk::PipelineLayoutCreateInfo pipeline_layout_create_info{};
		pipeline_layout_create_info.setLayoutCount = 0;
		pipeline_layout_create_info.pushConstantRangeCount = 0;
		auto layout = mDevice.createPipelineLayout(pipeline_layout_create_info);

		auto shader = ShaderManager::Get().Load("C:/Users/dariu/Documents/BHive/Runtime/Triangle.glsl");

		auto &stages = shader->GetStageCreateInfos();

		vk::PipelineRenderingCreateInfo pipeline_renderingCreateInfo{};
		pipeline_renderingCreateInfo.colorAttachmentCount = 1;
		pipeline_renderingCreateInfo.pColorAttachmentFormats = &mSwapChainImageFormat.format;

		vk::GraphicsPipelineCreateInfo pipeline_info{};
		pipeline_info.pNext = &pipeline_renderingCreateInfo;
		pipeline_info.stageCount = static_cast<uint32_t>(stages.size());
		pipeline_info.pStages = stages.data();
		pipeline_info.pVertexInputState = &vertexInputInfo;
		pipeline_info.pInputAssemblyState = &inputAssemblyInfo;
		pipeline_info.pViewportState = &viewportStateInfo;
		pipeline_info.pRasterizationState = &rasterizerInfo;
		pipeline_info.pMultisampleState = &multismapling;
		pipeline_info.pColorBlendState = &blend_state_create_info;
		pipeline_info.pDynamicState = &dynamicStateInfo;
		pipeline_info.layout = layout;
		pipeline_info.renderPass = nullptr;

		mGraphicsPipeline = mDevice.createGraphicsPipeline(nullptr, pipeline_info);
	}

	void GraphicsContext::CreateCommandPool()
	{
		vk::CommandPoolCreateInfo pool_info;
		pool_info.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
		pool_info.queueFamilyIndex = mQueueIndex;

		mCommandPool = vk::raii::CommandPool(mDevice, pool_info);
	}

	void GraphicsContext::CreateCommandBuffers()
	{
		mCommandBuffers.clear();

		vk::CommandBufferAllocateInfo alloc_info;
		alloc_info.commandPool = *mCommandPool;
		alloc_info.level = vk::CommandBufferLevel::ePrimary;
		alloc_info.commandBufferCount = MAX_FRAMES_IN_FLIGHT;
		mCommandBuffers = vk::raii::CommandBuffers(mDevice, alloc_info);
	}

	void GraphicsContext::RecordCommandBuffer(uint32_t imageIndex)
	{
		auto &cmd = mCommandBuffers[mCurrentFrame];
		cmd.begin({});

		transition_image_layout(
			imageIndex, vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal, {}, vk::AccessFlagBits2::eColorAttachmentWrite, vk::PipelineStageFlagBits2::eTopOfPipe,
			vk::PipelineStageFlagBits2::eColorAttachmentOutput);

		vk::ClearValue clearColor = vk::ClearColorValue(1.f, 0.f, 0.f, 1.f);
		vk::RenderingAttachmentInfo attachmentInfo;
		attachmentInfo.imageView = mSwapChainImageViews[imageIndex];
		attachmentInfo.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
		attachmentInfo.loadOp = vk::AttachmentLoadOp::eClear;
		attachmentInfo.storeOp = vk::AttachmentStoreOp::eStore;
		attachmentInfo.clearValue = clearColor;

		vk::RenderingInfo renderingInfo;
		renderingInfo.renderArea = vk::Rect2D({0, 0}, mSwapChainExtent);
		renderingInfo.layerCount = 1;
		renderingInfo.colorAttachmentCount = 1;
		renderingInfo.pColorAttachments = &attachmentInfo;

		cmd.beginRendering(renderingInfo);
		cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, mGraphicsPipeline);
		cmd.setViewport(0, vk::Viewport(0.0f, 0.0f, static_cast<float>(mSwapChainExtent.width), static_cast<float>(mSwapChainExtent.height), 0.0f, 1.0f));
		cmd.setScissor(0, vk::Rect2D({0, 0}, mSwapChainExtent));
		cmd.draw(3, 1, 0, 0);
		cmd.endRendering();

		transition_image_layout(
			imageIndex, vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::ePresentSrcKHR, vk::AccessFlagBits2::eColorAttachmentWrite, {}, vk::PipelineStageFlagBits2::eColorAttachmentOutput,
			vk::PipelineStageFlagBits2::eBottomOfPipe);

		cmd.end();
	}

	void GraphicsContext::CreateSyncObjects()
	{
		mPresetCompleteSemaphores.clear();
		mRenderFinishedSemaphores.clear();
		mInFlightFences.clear();

		for (size_t i = 0; i < mSwapChainImages.size(); i++)
		{
			mPresetCompleteSemaphores.emplace_back(mDevice, vk::SemaphoreCreateInfo{});
			mRenderFinishedSemaphores.emplace_back(mDevice, vk::SemaphoreCreateInfo{});
		}

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			mInFlightFences.emplace_back(mDevice, vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
		}
	}

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
		vk::ImageMemoryBarrier2 barrier{};
		barrier.srcStageMask = srcStageMask;
		barrier.srcAccessMask = srcAccessMask;
		barrier.dstStageMask = dstStageMask;
		barrier.dstAccessMask = dstAccessMask;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = mSwapChainImages[imageIndex];
		barrier.subresourceRange = {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1};

		vk::DependencyInfo depInfo{};
		depInfo.dependencyFlags = {};
		depInfo.imageMemoryBarrierCount = 1;
		depInfo.pImageMemoryBarriers = &barrier;
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
		for (uint32_t qfpIndex = 0; qfpIndex < static_cast<uint32_t>(queueFamilyProperties.size()); qfpIndex++)
		{
			if ((queueFamilyProperties[qfpIndex].queueFlags & vk::QueueFlagBits::eGraphics) && mPhysicalDevice.getSurfaceSupportKHR(qfpIndex, *mSurface))
			{
				mQueueIndex = qfpIndex;
				break;
			}
		}

		if (mQueueIndex == ~0)
		{
			LOG_ERROR("Failed to find a suitable queue family!");
			ASSERT(false);
		}

		auto queue_priority = 0.0f;

		vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan11Features, vk::PhysicalDeviceVulkan13Features, vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT> featureChain;
		featureChain.assign<vk::PhysicalDeviceFeatures2>({}); // default initialize all features to false
		featureChain.get<vk::PhysicalDeviceVulkan11Features>().shaderDrawParameters = true;
		featureChain.get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering = true;
		featureChain.get<vk::PhysicalDeviceVulkan13Features>().synchronization2 = true;
		featureChain.get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().extendedDynamicState = true;

		vk::DeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.queueFamilyIndex = mQueueIndex;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queue_priority;

		vk::DeviceCreateInfo device_createInfo{};
		device_createInfo.pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>();
		device_createInfo.queueCreateInfoCount = 1;
		device_createInfo.pQueueCreateInfos = &queueCreateInfo;
		device_createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredDeviceExtensions.size());
		device_createInfo.ppEnabledExtensionNames = requiredDeviceExtensions.data();

		mDevice = mPhysicalDevice.createDevice(device_createInfo);

		mQueue = mDevice.getQueue(mQueueIndex, 0);
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
		ASSERT(!availableFormats.empty());

		auto formatItr = std::ranges::find_if(availableFormats, [](auto format) { return format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear; });
		return formatItr != availableFormats.end() ? *formatItr : availableFormats[0];
	}

	vk::PresentModeKHR GraphicsContext::ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes)
	{
		ASSERT(std::ranges::any_of(availablePresentModes, [](auto mode) { return mode == vk::PresentModeKHR::eFifo; }));
		return std::ranges::any_of(availablePresentModes, [](auto mode) { return mode == vk::PresentModeKHR::eMailbox; }) ? vk::PresentModeKHR::eMailbox : vk::PresentModeKHR::eFifo;
	}

	vk::Extent2D GraphicsContext::ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities)
	{
		if (capabilities.currentExtent.width != 0xFFFFFFF)
		{
			return capabilities.currentExtent;
		}

		int width, height;
		glfwGetFramebufferSize(mWindowHandle, &width, &height);

		vk::Extent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
		return actualExtent;
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