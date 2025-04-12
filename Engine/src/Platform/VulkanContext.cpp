#include "core/Application.h"
#include "core/Window.h"
#include "VulkanContext.h"
#include "VulkanCore.h"
#include "VulkanInstance.h"
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
	}

	void VulkanContext::Init()
	{
		mInstance = CreateRef<VulkanInstance>(glfwGetWindowTitle(mWindowHandle));
		mSurface = CreateRef<VulkanSurface>(mInstance, mWindowHandle);

		uint32_t extension_count = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);

		std::vector<VkExtensionProperties> extensions(extension_count);
		vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data());

		LOG_INFO("Vulkan Extensions:")
		for (auto &ext : extensions)
		{
			LOG_INFO("\t {}", ext.extensionName);
		}

		ASSERT(mInstance);
		mDevice = CreateRef<VulkanDevice>(*mInstance);
	}

	VulkanInstance &VulkanContext::GetInstance()
	{
		return *Application::Get().GetWindow().GetContext().mInstance;
	}
} // namespace BHive