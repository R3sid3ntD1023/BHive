#include "VulkanWindowContext.h"
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <vulkan/vulkan_to_string.hpp>
#include "VulkanSwapChain.h"
#include "gfx/RenderCommand.h"
#include "VulkanRendererAPI.h"

namespace BHive
{
	VulkanWindowContext::VulkanWindowContext(void *windowHandle)
		: mDevice(VulkanBackend::GetLogicalDevice()),
		mWindowHandle(static_cast<GLFWwindow *>(windowHandle))
		 
	{
		ASSERT(mWindowHandle, "Window handle is null!");
	}

	VulkanWindowContext::~VulkanWindowContext()
	{
		LOG_TRACE("GraphicsContext Destructor Called")

		auto api = RenderCommand::GetRendererAPI<VulkanRendererAPI>();
		if (api->GetCurrentContext() == this)
			api->SetCurrentContext(nullptr);
	}

	void VulkanWindowContext::OnFramebufferResized(uint32_t w, uint32_t h)
	{
		mFramebufferResized = true;
	}

	void VulkanWindowContext::Init()
	{
		VulkanBackend::Get().Init();

		VkSurfaceKHR _surface;
		auto &instance = VulkanBackend::GetInstance();
		if (glfwCreateWindowSurface(*instance, mWindowHandle, nullptr, &_surface) != VK_SUCCESS)
		{
			LOG_ERROR("Failed to create window surface!");
			ASSERT(false);
		}

		mSurface = vk::raii::SurfaceKHR(instance, _surface);

		VulkanBackend::Get().CreateLogicalDevice(mSurface);
		VulkanBackend::Get().EnsurePresentSupportForSurface(*mSurface);

		CreateSwapChain();
		CreateCommandBuffers();
	}

	void VulkanWindowContext::SwapBuffers()
	{
		ASSERT(mCurrentFrame < mCommandBuffers.size());

		auto api = RenderCommand::GetRendererAPI<VulkanRendererAPI>();
		api->SetCurrentContext(this);

		auto result = api->RenderFrame(this);

		if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR || mFramebufferResized)
		{
			RecreateSwapChain();
			mFramebufferResized = false;
			return;
		}
		else if (result != vk::Result::eSuccess)
		{
			ASSERT(false, "Failed to present swap chain image!")
		}



		mCurrentFrame = (mCurrentFrame + 1) % VulkanBackend::MAX_FRAMES_IN_FLIGHT; 
	}

	void VulkanWindowContext::CreateCommandBuffers()
	{
		auto graphics_queue_index = VulkanBackend::GetQueueFamilies().GraphicsQueueIndex;

		vk::CommandPoolCreateInfo pool_info(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, graphics_queue_index);
		mCommandPool = vk::raii::CommandPool(mDevice, pool_info);

		vk::CommandBufferAllocateInfo alloc_info(mCommandPool, vk::CommandBufferLevel::ePrimary, VulkanBackend::MAX_FRAMES_IN_FLIGHT);
		mCommandBuffers = vk::raii::CommandBuffers(mDevice, alloc_info);
	}


	void VulkanWindowContext::CreateSwapChain()
	{
		int w = 0, h = 0;
		glfwGetFramebufferSize(mWindowHandle, &w, &h);

		while (w == 0 || h == 0)
		{
			glfwWaitEvents();
			glfwGetFramebufferSize(mWindowHandle, &w, &h);
		}

		auto &physical_device = VulkanBackend::GetPhysicalDevice();
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
		mSwapChain->Init(mSurface, create_info);
	}

	void VulkanWindowContext::RecreateSwapChain()
	{
		auto &device = VulkanBackend::GetLogicalDevice();
		device.waitIdle();

		CreateSwapChain();
		LOG_TRACE("Recreated swap chain!");
	}
} // namespace BHive