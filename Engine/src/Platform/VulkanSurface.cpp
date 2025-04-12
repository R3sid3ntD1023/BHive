#include "VulkanSurface.h"

// #define VK_USE_PLATFORM_WIN32_KHR
// #define GLFW_INCLUDE_VULKAN
//
// #define GLFW_EXPOSE_NATIVE_WIN32

// #include <GLFW/glfw3native.h>
#include <GLFW/glfw3.h>
// #include <vulkan/vulkan_win32.h>

namespace BHive
{
	VulkanSurface::VulkanSurface(const Ref<VulkanInstance> &instance, GLFWwindow *window)
		: mInstance(instance)
	{
		/*VkWin32SurfaceCreateInfoKHR createinfo{};
		createinfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		createinfo.hwnd = glfwGetWin32Window(window);
		createinfo.hinstance = GetModuleHandle(nullptr);*/

		VK_ASSERT(glfwCreateWindowSurface(*instance, window, nullptr, &mSurface), "Failed to create window surface!");
	}

	VulkanSurface::~VulkanSurface()
	{
		vkDestroySurfaceKHR(*mInstance, mSurface, nullptr);
	}
} // namespace BHive