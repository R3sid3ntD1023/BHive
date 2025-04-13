#pragma once

#include "core/Core.h"
#include "VulkanCore.h"

struct GLFWwindow;

namespace BHive
{
	using Queues = std::vector<VkQueue>;
	using DeviceExtensions = std::vector<const char *>;
	using SurfaceFormats = std::vector<VkSurfaceFormatKHR>;
	using PresentModes = std::vector<VkPresentModeKHR>;

	struct QueueFamilyIndices
	{
		std::optional<uint32_t> Graphics;
		std::optional<uint32_t> Present;

		std::array<uint32_t, 2> GetIndices() const { return {Graphics.value(), Present.value()}; }
		bool IsValid() const { return Graphics.has_value() && Present.has_value(); }
	};

	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR Capabilities;
		SurfaceFormats Formats;
		PresentModes PresentModes;
	};

	class VulkanAPI
	{
	public:
		void Init(const char *name, GLFWwindow *window);
		void Shutdown();

		VkDevice GetDevice() const { return mDevice; }
		VkSurfaceKHR GetSurface() const { return mSurface; }
		VkFormat GetSurfaceFormat() const { return mSurfaceFormat; }

		const SwapChainSupportDetails &GetSwapChainSupportDetails() const { return mSwapChainSupportDetails; }
		const QueueFamilyIndices &GetQueueFamilyIndices() const { return mQueueFamilyIndices; }

		static VulkanAPI &GetAPI()
		{
			ASSERT(sAPI, "API is Null");
			return *sAPI;
		}

	private:
		void CreateInstance(const char *name);
		void CreateLogialDevice();
		void GetPhysicalDevice();
		void CreateWindowSurface(GLFWwindow *window);

		bool CheckValidationLayerSupport();
		void PopulateDebugCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
		std::vector<const char *> GetRequiredExtensions();
		void CreateDebugMessenger();

		// device
		int32_t GetDeviceSuitability(VkPhysicalDevice device);
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
		bool IsDeviceSuitable(VkPhysicalDevice device);
		bool CheckExtensionSupport(VkPhysicalDevice device, const DeviceExtensions &exts);
		SwapChainSupportDetails QuerySwapChainSupportDetails(VkPhysicalDevice device);

	private:
		VkInstance mInstance = VK_NULL_HANDLE;
		VkDebugUtilsMessengerEXT mDebugMessenger = VK_NULL_HANDLE;
		VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
		VkDevice mDevice = VK_NULL_HANDLE;
		VkSurfaceKHR mSurface = VK_NULL_HANDLE;
		VkQueue mGraphicsQueue = VK_NULL_HANDLE;
		VkQueue mPresentQueue = VK_NULL_HANDLE;

		std::vector<const char *> mValidationLayers;
		QueueFamilyIndices mQueueFamilyIndices;
		DeviceExtensions mDeviceExtensions;
		SwapChainSupportDetails mSwapChainSupportDetails;

		VkExtent2D mSurfaceExtents;
		VkFormat mSurfaceFormat;

		static Scope<VulkanAPI> sAPI;
	};
} // namespace BHive