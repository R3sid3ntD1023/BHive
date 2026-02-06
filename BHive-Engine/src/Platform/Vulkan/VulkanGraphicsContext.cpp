#include "VulkanGraphicsContext.h"
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <vulkan/vulkan_to_string.hpp>
#include "VulkanUtils.h"
#include "VulkanSwapChain.h"
#include "gfx/RenderCommand.h"
#include "Platform/Vulkan/VulkanRendererAPI.h"

namespace BHive
{
	VulkanGraphicsContext::VulkanGraphicsContext(void *windowHandle)
		: mWindowHandle(static_cast<GLFWwindow *>(windowHandle))
	{
		ASSERT(mWindowHandle, "Window handle is null!");
	}

	VulkanGraphicsContext::~VulkanGraphicsContext()
	{
		VulkanCore::Shutdown();

	}

	void VulkanGraphicsContext::OnFramebufferResized(uint32_t w, uint32_t h)
	{
		mFramebufferResized = true;
	}

	void VulkanGraphicsContext::Init()
	{
		VulkanCore::Init();

		mSurface = VulkanCore::CreateSurface(mWindowHandle);

		VulkanCore::CreateLogicalDevice(mSurface);
		VulkanCore::EnsurePresentSupportForSurface(*mSurface);

		CreateSwapChain();
	}

	void VulkanGraphicsContext::SwapBuffers()
	{
		auto api = RenderCommand::GetAPI<VulkanRendererAPI>();
		auto [result, imageIndex] = mSwapChain->AquireNextImage(mCurrentFrame);
		auto &image = mSwapChain->GetImage(imageIndex);
		auto &image_view = mSwapChain->GetImageView(imageIndex);
		auto extent = mSwapChain->GetExtent();

		if (result == vk::Result::eErrorOutOfDateKHR)
		{
			RecreateSwapChain();
			return;
		}

		ASSERT(result == vk::Result::eSuccess || result == vk::Result::eSuboptimalKHR, "Failed to acquire swap chain image!");


		auto& cmd = api->RenderFrame(mCurrentFrame, imageIndex, image, image_view, extent);

		result = mSwapChain->Present(cmd, imageIndex, mCurrentFrame);

		if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR || mFramebufferResized)
		{
			mFramebufferResized = false;
			RecreateSwapChain();
		}
		else if (result != vk::Result::eSuccess)
		{
			ASSERT(false, "Failed to present swap chain image!")
		}

		api->GetFrameResources(mCurrentFrame).StagingBuffers.clear();

		mCurrentFrame = (mCurrentFrame + 1) % VulkanCore::MAX_FRAMES_IN_FLIGHT;
	}

	void VulkanGraphicsContext::CreateSwapChain()
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
		create_info.OldSwapChain = mSwapChain ? ***mSwapChain : VK_NULL_HANDLE;

		mSwapChain = CreateRef<VulkanSwapChain>();
		mSwapChain->Init(mSurface, create_info);
	}

	void VulkanGraphicsContext::RecreateSwapChain()
	{
		int width = 0, height = 0;
		glfwGetFramebufferSize(mWindowHandle, &width, &height);
		while (width == 0 || height == 0)
		{
			glfwWaitEvents();
			glfwGetFramebufferSize(mWindowHandle, &width, &height);
			
		}

		auto &device = VulkanCore::GetLogicalDevice();
		device.waitIdle();

		CreateSwapChain();
		LOG_TRACE("Recreated swap chain!");
	}
} // namespace BHive