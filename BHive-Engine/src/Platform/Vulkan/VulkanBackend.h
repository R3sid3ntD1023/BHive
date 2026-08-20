#pragma once

#include "core/Core.h"
#include "VulkanCore.h"
#include "MemoryAllocator.h"
#include "GPUResourceManager.h"
#include "VulkanSwapChain.h"

struct GLFWwindow;

namespace BHive
{
	struct VkQueueFamilies
	{
		vk::raii::Queue PresentQueue = VK_NULL_HANDLE;
		vk::raii::Queue GraphicsQueue = VK_NULL_HANDLE;
		vk::raii::Queue ComputeQueue = VK_NULL_HANDLE;
		vk::raii::Queue TransferQueue = VK_NULL_HANDLE;

		uint32_t PresentQueueIndex = 0;
		uint32_t GraphicsQueueIndex = 0;
		uint32_t ComputeQueueIndex = 0;
		uint32_t TransferQueueIndex = 0;
	};

	class BHIVE_API VulkanBackend
	{
	public:
		~VulkanBackend();

		void Init();

		void Shutdown();

		std::vector<const char *> GetRequiredExtensions();

		uint32_t SelectQueueIndex(vk::QueueFlags queue_type, vk::SurfaceKHR surface);

		uint32_t SelectQueueIndex(vk::QueueFlags queue_type);

		void CreateLogicalDevice();

		bool EnsurePresentSupportForSurface(vk::SurfaceKHR surface);

		void CreatePerImageSync(uint32_t imgCount);

		vk::Result Present();

		bool RequestSwapChainRecreate(uint32_t w, uint32_t h);

		template <typename THandleType>
		static void SetObjectName(const THandleType &handle, const std::string &name)
		{
#ifdef _DEBUG
			auto &device = VulkanBackend::GetLogicalDevice();
			device.setDebugUtilsObjectNameEXT<THandleType>(handle, name);

			VkObjectType type = (VkObjectType)handle.objectType;
			uint64_t h = reinterpret_cast<uint64_t>(&handle);
			GetDebugNameRegistry().SetName(type, h, name);
#endif
		}

		static VulkanBackend &Get() { return *sInstance; }

		static vk::DescriptorPool GetDescriptorPool() { return Get().mDescriptorPool; }

		static vk::raii::Device &GetLogicalDevice() { return Get().mDevice; }

		static vk::raii::PhysicalDevice &GetPhysicalDevice() { return Get().mPhysicalDevice; }

		static const VkQueueFamilies &GetQueueFamilies() { return Get().mQueueFamilies; }

		static vk::raii::Instance &GetInstance() { return Get().mInstance; }

		static vk::raii::CommandPool &GetImmediateCommandPool() { return Get().mImmediateCommandPool; }

		static MemoryAllocator &GetMemoryAllocator() { return *Get().mMemoryAllocator; }

		static GPUResourceManager &GetGPUResourceManager() { return *Get().mGPUResourceManager; }

		static vk::raii::CommandPool &GetCommandPool() { return Get().mCommandPool; }

		static vk::raii::CommandBuffer &GetCommandBuffer(uint32_t frame) { return Get().mCommandBuffers.at(frame); }

		struct DebugNameRegistry
		{
			std::unordered_map<uint64_t, std::string> Names;

			void SetName(VkObjectType type, uint64_t handle, const std::string &name)
			{
				uint64_t key = (uint64_t(type) << 56) | handle;
				Names[key] = name;
			}

			std::string GetName(vk::ObjectType type, uint64_t handle) const
			{
				uint64_t key = (uint64_t(type) << 56) | handle;
				return Names.contains(key) ? Names.at(key) : "<unnamed>";
			}
		};

		static DebugNameRegistry &GetDebugNameRegistry() { return Get().mDebugNames; }

	private:
		void CreateInstance();

		void CreateDebugMessenger();

		void PickPhysicalDevice();

		void CreateCommandBuffers();

		void CreateImmediateCommandPool();

		void CreateDescriptorPool();

		void CreateMemoryAllocator();

		void CreateGPUResourceManager();

		void CreateDeviceInternal(uint32_t graphicsIndex, uint32_t presentIndex);

	private:
		vk::raii::Context mContext;

		vk::raii::Instance mInstance = nullptr;

		vk::raii::PhysicalDevice mPhysicalDevice = nullptr;

		vk::raii::Device mDevice = nullptr;

		vk::raii::DebugUtilsMessengerEXT mDebugMessenger = nullptr;

		vk::raii::CommandPool mImmediateCommandPool = nullptr;

		vk::raii::CommandPool mCommandPool = nullptr;

		vk::raii::CommandBuffers mCommandBuffers = nullptr;

		vk::raii::DescriptorPool mDescriptorPool = VK_NULL_HANDLE;

		VkQueueFamilies mQueueFamilies;

		Scope<MemoryAllocator> mMemoryAllocator;

		Scope<GPUResourceManager> mGPUResourceManager;

		DebugNameRegistry mDebugNames;

		bool mInitialized = false;

		static inline VulkanBackend *sInstance = nullptr;
	};
} // namespace BHive