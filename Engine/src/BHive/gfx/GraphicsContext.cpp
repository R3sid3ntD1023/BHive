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

#define MAX_FRAMES_IN_FLIGHT 2

namespace BHive
{
	static bool sFramebufferResized = false;

	static void framebufferResizeCallback(GLFWwindow *window, int width, int height)
	{
		sFramebufferResized = true;
	}

	GraphicsContext::GraphicsContext(GLFWwindow *window)
		: mWindowHandle(window)
	{
		ASSERT(!sInstance);
		sInstance = this;
		ASSERT(sInstance);

		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

		VulkanCore::Init();
	}

	GraphicsContext::~GraphicsContext()
	{
		mDevice.waitIdle();
	}

	void GraphicsContext::Init()
	{

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
		auto &command_buffer = api->GetCurrentCommandBuffer();
		auto &buffers = api->GetCommandBuffers();

		mSwapChain->ResetCommandBuffer(command_buffer);

		RenderCommand::BeginFrame();

		RenderCommand::EndFrame();

		std::vector<vk::CommandBuffer> buffers_to_submit;
		for (auto &cmd : buffers)
		{
			buffers_to_submit.push_back(cmd.at(current_frame));
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

	void GraphicsContext::CreateLogicalDevice()
	{

		mQueueFamilies.GraphicsQueueIndex = VulkanCore::SelectQueueIndex(vk::QueueFlagBits::eGraphics, *mSurface);
		if (mQueueFamilies.GraphicsQueueIndex == ~0)
		{
			LOG_ERROR("Failed to find a suitable queue family!");
			ASSERT(false);
		}

		auto queue_priority = 0.0f;
		auto requiredDeviceExtensions = VulkanCore::GetRequiredExtensions();

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

		auto &physical_device = VulkanCore::GetPhysicalDevice();
		mDevice = physical_device.createDevice(device_createInfo);

		mQueueFamilies.GraphicsQueue = mDevice.getQueue(mQueueFamilies.GraphicsQueueIndex, 0);
	}

	void GraphicsContext::CreateSwapChain()
	{
		int w = 0, h = 0;
		glfwGetFramebufferSize(mWindowHandle, &w, &h);

		auto &physical_device = VulkanCore::GetPhysicalDevice();
		auto surfaceCapabilities = physical_device.getSurfaceCapabilitiesKHR(*mSurface);
		auto formats = physical_device.getSurfaceFormatsKHR(*mSurface);
		auto presentModes = physical_device.getSurfacePresentModesKHR(*mSurface);

		VulkanSwapChainCreateInfo create_info{};
		create_info.Width = w;
		create_info.Height = h;
		create_info.Capabilities = surfaceCapabilities;
		create_info.Formats = formats;
		create_info.PresentModes = presentModes;
		mSwapChain = CreateRef<VulkanSwapChain>();
		mSwapChain->Init(mDevice, mSurface, create_info);
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
		CreateSwapChain();
	}

	void GraphicsContext::CreateSurface()
	{
		VkSurfaceKHR _surface;
		auto &instance = VulkanCore::GetInstance();
		if (glfwCreateWindowSurface(*instance, mWindowHandle, nullptr, &_surface) != VK_SUCCESS)
		{
			LOG_ERROR("Failed to create window surface!");
			ASSERT(false);
		}
		mSurface = vk::raii::SurfaceKHR(instance, _surface);
	}

} // namespace BHive