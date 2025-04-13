#include "core/Application.h"
#include "core/Window.h"
#include "VulkanAPI.h"
#include "VulkanContext.h"
#include "VulkanCore.h"
#include "VulkanSwapChain.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace BHive
{
	VulkanContext::VulkanContext(GLFWwindow *window)
		: mWindowHandle(window)
	{
		ASSERT(mWindowHandle, "WindowHandle was null!");
	}

	VulkanContext::~VulkanContext()
	{
		VulkanAPI::GetAPI().Shutdown();
	}

	void VulkanContext::Init()
	{
		VulkanAPI::GetAPI().Init(glfwGetWindowTitle(mWindowHandle), mWindowHandle);

		uint32_t extension_count = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);

		std::vector<VkExtensionProperties> extensions(extension_count);
		vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data());

		LOG_INFO("Vulkan Extensions:")
		for (auto &ext : extensions)
		{
			LOG_INFO("\t {}", ext.extensionName);
		}

		mSwapChain = CreateRef<VulkanSwapChain>();
	}

} // namespace BHive