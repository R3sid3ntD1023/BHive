#pragma once

#include "core/Core.h"
#include "VulkanCore.h"

struct GLFWwindow;

namespace BHive
{
	struct QueueFamilyIndices
	{
	};

	class BHIVE_API GraphicsContext
	{
	public:
		GraphicsContext(GLFWwindow *window);

		virtual ~GraphicsContext();

		virtual void Init();

		virtual void SwapBuffers();

		void transition_image_layout(
			uint32_t imageIndex, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::AccessFlags2 srcAccessMask, vk::AccessFlags2 dstAccessMask, vk::PipelineStageFlags2 srcStageMask,
			vk::PipelineStageFlags2 dstStageMask);

		vk::raii::Instance &GetInstance() { return mVulkanInstance; }

		vk::raii::Device &GetDevice() { return mDevice; }

		vk::raii::PhysicalDevice &GetPhysicalDevice() { return mPhysicalDevice; }

		vk::raii::CommandPool &GetCommandPool() { return mCommandPool; };

		vk::raii::Queue &GetGraphicsQueue() { return mQueue; }

		static GraphicsContext &Get()
		{
			ASSERT(sInstance);
			return *sInstance;
		}

	private:
		void CreateIntance();

		void CreateDebugMessenger();

		void PickPhysicalDevice();

		void CreateSwapChain();

		void CreateImageViews();

		void CreateGraphicsPipeline();

		void CreateCommandPool();

		void CreateCommandBuffers();

		void RecordCommandBuffer(uint32_t imageIndex);

		void CreateSyncObjects();

		void RecreateSwapChain();

		void CleanupSwapChain();

		uint32_t FindQueueFamilies(vk::PhysicalDevice device);

		void CreateLogicalDevice();

		void CreateSurface();

		vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats);

		vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes);

		vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities);

		uint32_t ChooseMinImageCount(vk::SurfaceCapabilitiesKHR capabilities);

		void CreateVertexBuffer();

		void CreateIndexBuffer();

		void CreateUniformBuffers();

		void CreateDescriptorSetLayout();

		void CreateDescriptorPool();

		void CreateDescriptorSets();

		void CreateTextureImage();

		void UpdateUniformBuffer(uint32_t currentImage);

	private:
		GLFWwindow *mWindowHandle;

		vk::raii::Context mVulkanContext;

		vk::raii::Instance mVulkanInstance = nullptr;

		vk::raii::DebugUtilsMessengerEXT mDebugMessenger = nullptr;

		vk::raii::PhysicalDevice mPhysicalDevice = nullptr;

		vk::raii::Device mDevice = nullptr;

		vk::raii::Queue mQueue = nullptr;

		vk::raii::SurfaceKHR mSurface = nullptr;

		vk::Extent2D mSwapChainExtent;

		vk::SurfaceFormatKHR mSwapChainImageFormat;

		vk::raii::SwapchainKHR mSwapChain = nullptr;

		std::vector<vk::Image> mSwapChainImages{};

		std::vector<vk::raii::ImageView> mSwapChainImageViews{};

		uint32_t mQueueIndex = 0;

		vk::raii::CommandPool mCommandPool = nullptr;

		std::vector<vk::raii::CommandBuffer> mCommandBuffers{};

		std::vector<vk::raii::Semaphore> mPresetCompleteSemaphores{};

		std::vector<vk::raii::Semaphore> mRenderFinishedSemaphores{};

		std::vector<vk::raii::Fence> mInFlightFences{};

		vk::raii::Pipeline mGraphicsPipeline = nullptr;

		vk::raii::DescriptorSetLayout mDescriptorSetLayout = nullptr;

		vk::raii::PipelineLayout mPipelineLayout = nullptr;

		uint32_t mCurrentFrame = 0;

		vk::raii::Buffer mVertexBuffer = nullptr;

		vk::raii::DeviceMemory mVertexBufferMemory = nullptr;

		vk::raii::Buffer mIndexBuffer = nullptr;

		vk::raii::DeviceMemory mIndexBufferMemory = nullptr;

		std::vector<vk::raii::Buffer> mUniformBuffers{};
		std::vector<vk::raii::DeviceMemory> mUniformBuffersMemory{};
		std::vector<void *> mUniformBuffersMapped{};

		vk::raii::DescriptorPool mDescriptorPool = nullptr;
		std::vector<vk::raii::DescriptorSet> mDescriptorSets{};

		Ref<class Texture2D> mTexture;

		static inline GraphicsContext *sInstance = nullptr;
	};
} // namespace BHive