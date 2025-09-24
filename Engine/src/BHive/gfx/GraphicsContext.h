#pragma once

#include "core/Core.h"
#include "vulkan/vulkan_raii.hpp"

struct GLFWwindow;

namespace BHive
{

	class BHIVE_API GraphicsContext
	{
	public:
		GraphicsContext(GLFWwindow *window);

		virtual ~GraphicsContext() = default;

		virtual void Init();

		virtual void SwapBuffers();

		vk::raii::Instance &GetInstance() { return mVulkanInstance; }

	private:
		GLFWwindow *mWindowHandle;

		vk::raii::Context mVulkanContext;

		vk::raii::Instance mVulkanInstance = nullptr;

		vk::raii::DebugUtilsMessengerEXT mDebugMessenger = nullptr;

		Scope<vk::raii::PhysicalDevice> mPhysicalDevice = nullptr;

		vk::raii::Device mDevice = nullptr;
	};
} // namespace BHive