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
		static void SetObjectName(const THandleType& handle, const std::string &name)
		{
#ifdef _DEBUG
			auto &device = VulkanBackend::GetLogicalDevice();
			device.setDebugUtilsObjectNameEXT<THandleType>(handle, name);
			
			VkObjectType type = (VkObjectType)handle.objectType;
			uint64_t h = reinterpret_cast<uint64_t>(&handle);
			GetDebugNameRegistry().SetName(type, h, name);
#endif
		}

		static VulkanBackend &Get()
		{
			static VulkanBackend sBackend;
			return sBackend;
		}


		static vk::raii::Device &GetLogicalDevice() { return Get().mDevice; }

		static vk::raii::PhysicalDevice &GetPhysicalDevice() { return Get().mPhysicalDevice; }

		static const VkQueueFamilies &GetQueueFamilies() { return Get().mQueueFamilies; }

		static vk::raii::Instance &GetInstance() { return Get().mInstance; }

		static vk::raii::CommandPool &GetImmediateCommandPool() { return Get().mImmediateCommandPool; }

		static MemoryAllocator &GetMemoryAllocator() { return *Get().mMemoryAllocator; }

		static GPUResourceManager &GetGPUResourceManager() { return *Get().mGPUResourceManager; }

		struct DebugNameRegistry
		{
			std::unordered_map<uint64_t, std::string> Names;

			void SetName(VkObjectType type, uint64_t handle, const std::string &name)
			{
				uint64_t key = (uint64_t(type) << 56) | handle;
				Names[key] = name;
			}

			std::string GetName(VkObjectType type, uint64_t handle) const
			{
				uint64_t key = (uint64_t(type) << 56) | handle;
				return Names.contains(key) ? Names.at(key) : "<unnamed>";
			}
		};

		static DebugNameRegistry& GetDebugNameRegistry() { return Get().mDebugNames; }

	private:
		void CreateIntance();

		void CreateDebugMessenger();

		void PickPhysicalDevice();

		void CreateImmediateCommandPool();

		void CreateMemoryAllocator();

		void CreateGPUResourceManager();

		void CreateDeviceInternal(uint32_t graphicsIndex, uint32_t presentIndex);

		
	private:
		vk::raii::Context mContext;

		vk::raii::Instance mInstance = nullptr;

		vk::raii::DebugUtilsMessengerEXT mDebugMessenger = nullptr;

		vk::raii::PhysicalDevice mPhysicalDevice = nullptr;

		vk::raii::Device mDevice = nullptr;

		vk::raii::CommandPool mImmediateCommandPool = nullptr;

		VkQueueFamilies mQueueFamilies;

		bool mInitialized = false;

		std::vector<DeviceCallback> mOnDeviceDestroyedCallbacks;

		std::vector<DeviceCallback> mOnDeviceCreatedCallbacks;

		Scope<MemoryAllocator> mMemoryAllocator;

		Scope<GPUResourceManager> mGPUResourceManager;

		DebugNameRegistry mDebugNames;
	};

	
} // namespace BHive