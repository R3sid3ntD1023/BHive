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
		void Init(GLFWwindow* window);

		void Shutdown();

		std::vector<const char *> GetRequiredExtensions();

		uint32_t SelectQueueIndex(vk::QueueFlags queue_type, const vk::SurfaceKHR &surface);

		uint32_t SelectQueueIndex(vk::QueueFlags queue_type);

		void CreateLogicalDevice(const vk::SurfaceKHR &surface);

		void EnsurePresentSupportForSurface(const vk::SurfaceKHR &surface);

		void CreatePerImageSync(uint32_t imgCount);

		vk::Result Present();

		void RequestSwapChainRecreate(uint32_t w, uint32_t h);

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

		static vk::raii::CommandPool& GetCommandPool() { return Get().mCommandPool; }

		static vk::raii::CommandBuffer& GetCommandBuffer(uint32_t frame) { return Get().mCommandBuffers.at(frame); }

		static VulkanSwapChain &GetSwapChain() { return *Get().mSwapChain; }

		static vk::Semaphore GetRenderFinishedSemaphore(uint32_t imageIndex);

		static vk::Semaphore GetImageAvailableSemaphore(uint32_t frame);

		static vk::Fence GetInFlightFence(uint32_t frame);

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

		static DebugNameRegistry& GetDebugNameRegistry() { return Get().mDebugNames; }

	private:
		void CreateIntance();

		void CreateDebugMessenger();

		void PickPhysicalDevice();

		void CreateWindowSurface(GLFWwindow *window);

		void CreateSwapChain(GLFWwindow* window);

		void CreateSyncObjects();

		
		void CreateCommandBuffers();

		void CreateImmediateCommandPool();

		void CreateMemoryAllocator();

		void CreateGPUResourceManager();

		void CreateDeviceInternal(uint32_t graphicsIndex, uint32_t presentIndex);

		void RecreateFrameResources();

	private:
		vk::raii::Context mContext;

		vk::raii::Instance mInstance = nullptr;

		vk::raii::DebugUtilsMessengerEXT mDebugMessenger = nullptr;

		vk::raii::PhysicalDevice mPhysicalDevice = nullptr;

		vk::raii::Device mDevice = nullptr;

		vk::raii::CommandPool mImmediateCommandPool = nullptr;

		vk::raii::SurfaceKHR mSurface = nullptr;

		vk::raii::CommandPool mCommandPool = nullptr;

		vk::raii::CommandBuffers mCommandBuffers = nullptr;

		std::vector<vk::raii::Semaphore> mPresentSemaphores; // per frame
		std::vector<vk::raii::Semaphore> mRenderFinishedSemaphores; // per image
		std::vector<vk::raii::Fence> mInFlightFences; // per frame

		VkQueueFamilies mQueueFamilies;

		bool mInitialized = false;

		Scope<MemoryAllocator> mMemoryAllocator;

		Scope<GPUResourceManager> mGPUResourceManager;

		DebugNameRegistry mDebugNames;

		Scope<VulkanSwapChain> mSwapChain;
	};
} // namespace BHive