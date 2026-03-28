#pragma once

#include "core/Core.h"
#include "VulkanCore.h"
#include "MemoryAllocator.h"
#include "GPUResourceManager.h"

struct GLFWwindow;

namespace BHive
{
	struct VkQueueFamilies
	{
		vk::raii::Queue PresentQueue = VK_NULL_HANDLE;
		vk::raii::Queue GraphicsQueue = VK_NULL_HANDLE;
		vk::raii::Queue ComputeQueue = VK_NULL_HANDLE;
		vk::raii::Queue TransferQueue = VK_NULL_HANDLE;

		int32_t PresentQueueIndex = -1;
		int32_t GraphicsQueueIndex = -1;
		int32_t ComputeQueueIndex = -1;
		int32_t TransferQueueIndex = -1;
	};


	class BHIVE_API VulkanBackend
	{
	public:
		using DeviceCallback = std::function<void()>;

	public:
		void Init();

		void Shutdown();

		std::vector<const char *> GetRequiredExtensions();

		uint32_t SelectQueueIndex(vk::QueueFlags queue_type, const vk::SurfaceKHR &surface);

		uint32_t SelectQueueIndex(vk::QueueFlags queue_type);

		void CreateLogicalDevice(const vk::SurfaceKHR &surface);

		void EnsurePresentSupportForSurface(const vk::SurfaceKHR &surface);

		// device lifecycle callbacks
		void RegisterOnDeviceCreated(const DeviceCallback &callback);

		void RegisterOnDeviceDestroy(const DeviceCallback &callback);

		void CallOnDeviceDestroyed();

		template <typename THandleType>
		static void SetObjectName(THandleType handle, const std::string &name)
		{
			auto &device = VulkanBackend::GetLogicalDevice();
			device.setDebugUtilsObjectNameEXT<THandleType>(handle, name);
		}

		static VulkanBackend &Get()
		{
			static VulkanBackend sBackend;
			return sBackend;
		}


		static vk::raii::Device &GetLogicalDevice() { return Get().mLogicalDevice; }

		static vk::raii::PhysicalDevice &GetPhysicalDevice() { return Get().mPhysicalDevice; }

		static const VkQueueFamilies &GetQueueFamilies() { return Get().mQueueFamilies; }

		static vk::raii::Instance &GetInstance() { return Get().mVulkanInstance; }

		static vk::raii::CommandPool &GetImmediateCommandPool() { return Get().mImmediateCommandPool; }

		static MemoryAllocator &GetMemoryAllocator() { return *Get().mMemoryAllocator; }

		static GPUResourceManager &GetGPUResourceManager() { return *Get().mGPUResourceManager; }


	private:
		void CreateIntance();

		void CreateDebugMessenger();

		void PickPhysicalDevice();

		void CreateImmediateCommandPool();

		void CreateMemoryAllocator();

		void CreateGPUResourceManager();

		void CreateDeviceInternal(uint32_t graphicsIndex, uint32_t presentIndex);

	private:
		vk::raii::Context mVulkanContext;

		vk::raii::Instance mVulkanInstance = nullptr;

		vk::raii::DebugUtilsMessengerEXT mDebugMessenger = nullptr;

		vk::raii::PhysicalDevice mPhysicalDevice = nullptr;

		vk::raii::Device mLogicalDevice = nullptr;

		vk::raii::CommandPool mImmediateCommandPool = nullptr;

		VkQueueFamilies mQueueFamilies;

		bool mInitialized = false;

		std::vector<DeviceCallback> mOnDeviceDestroyedCallbacks;

		std::vector<DeviceCallback> mOnDeviceCreatedCallbacks;

		Scope<MemoryAllocator> mMemoryAllocator;

		Scope<GPUResourceManager> mGPUResourceManager;
	};

	
} // namespace BHive