#pragma once

#include "core/Core.h"
#include "core/EventDelegate.h"
#include "VulkanCore.h"

struct GLFWwindow;

namespace BHive
{
	class VulkanSwapChain;
	class VulkanDevice;
	class VulkanPipeline;

	class BHIVE_API GraphicsContext
	{
	public:
		struct FQueueFamilies
		{
			vk::raii::Queue PresentQueue = VK_NULL_HANDLE;
			vk::raii::Queue GraphicsQueue = VK_NULL_HANDLE;
			vk::raii::Queue ComputeQueue = VK_NULL_HANDLE;

			int32_t PresentQueueIndex = -1;
			int32_t GraphicsQueueIndex = -1;
			int32_t ComputeQueueIndex = -1;
		};

		GraphicsContext(GLFWwindow *window);

		virtual ~GraphicsContext();

		virtual void Init();

		virtual void SwapBuffers();

		const Ref<VulkanSwapChain>& GetSwapChain() const { return mSwapChain; }

		vk::raii::Instance &GetInstance() { return mVulkanInstance; }

		vk::raii::Device &GetDevice() { return mDevice; }

		vk::raii::PhysicalDevice &GetPhysicalDevice() { return mPhysicalDevice; }

		const FQueueFamilies &GetQueueFamilies() const { return mQueueFamilies; }

		uint32_t GetImageIndex() const { return mImageIndex; }

		static constexpr uint32_t GetInstanceVersion();

		static GraphicsContext &Get()
		{
			ASSERT(sInstance);
			return *sInstance;
		}

	private:
		
		void CreateIntance();

		void CreateDebugMessenger();

		void PickPhysicalDevice();

		void CreateLogicalDevice();

		void CreateSurface();

		void CreateSwapChain();

		std::vector<const char *> GetRequiredExtensions();

		void RecreateSwapChain();
			
	private:
		GLFWwindow *mWindowHandle;

		vk::raii::Context mVulkanContext;

		vk::raii::Instance mVulkanInstance = nullptr;

		vk::raii::DebugUtilsMessengerEXT mDebugMessenger = nullptr;

		vk::raii::PhysicalDevice mPhysicalDevice = nullptr;

		vk::raii::Device mDevice = nullptr;

		vk::raii::SurfaceKHR mSurface = nullptr;

		Ref<VulkanSwapChain> mSwapChain;

		FQueueFamilies mQueueFamilies{};

		static inline GraphicsContext *sInstance = nullptr;

		uint32_t mImageIndex = 0;
	};
} // namespace BHive