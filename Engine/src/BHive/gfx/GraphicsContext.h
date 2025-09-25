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

		vk::raii::Device &GetDevice() { return mDevice; }

	private:
		void CreateIntance();

		void CreateDebugMessenger();

		void PickPhysicalDevice();

		void CreateSwapChain();

		uint32_t FindQueueFamilies(vk::PhysicalDevice device);

		void CreateLogicalDevice();

		void CreateSurface();

		vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats);

		vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes);

		vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities);

		uint32_t ChooseMinImageCount(vk::SurfaceCapabilitiesKHR capabilities);

	private:
		GLFWwindow *mWindowHandle;

		vk::raii::Context mVulkanContext;

		vk::raii::Instance mVulkanInstance = nullptr;

		vk::raii::DebugUtilsMessengerEXT mDebugMessenger = nullptr;

		vk::raii::PhysicalDevice mPhysicalDevice = nullptr;

		vk::raii::Device mDevice = nullptr;

		vk::raii::Queue mGraphicsQueue = nullptr;

		vk::raii::Queue mPresentQueue = nullptr;

		vk::raii::SurfaceKHR mSurface = nullptr;

		vk::Extent2D mSwapChainExtent;

		vk::SurfaceFormatKHR mSwapChainImageFormat;

		vk::raii::SwapchainKHR mSwapChain = nullptr;

		std::vector<vk::Image> mSwapChainImages{};
	};
} // namespace BHive