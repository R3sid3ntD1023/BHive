#pragma once

#include "core/Core.h"
#include "VulkanCore.h"


struct GLFWwindow;

namespace BHive
{
	class VulkanSwapChain;

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

		

		vk::raii::Instance &GetInstance() { return mVulkanInstance; }

		vk::raii::Device &GetDevice() { return mDevice; }

		vk::raii::PhysicalDevice &GetPhysicalDevice() { return mPhysicalDevice; }

		vk::raii::CommandPool &GetCommandPool() { return mCommandPool; };

		vk::raii::Queue &GetGraphicsQueue() { return mQueue; }

		vk::raii::CommandBuffer &GetCommandBuffer() { return mCommandBuffers[mCurrentFrame]; }

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

		void CreateGraphicsPipeline();

		void CreateCommandPool();

		void CreateCommandBuffers();

		void RecordCommandBuffer(uint32_t imageIndex);

		void CreateSyncObjects();

		void RecreateSwapChain();

		void CreateLogicalDevice();

		void CreateSurface();

		

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

		static inline vk::raii::Instance mVulkanInstance = nullptr;

		vk::raii::DebugUtilsMessengerEXT mDebugMessenger = nullptr;

		vk::raii::PhysicalDevice mPhysicalDevice = nullptr;

		static inline vk::raii::Device mDevice = nullptr;

		vk::raii::Queue mQueue = nullptr;

		vk::raii::SurfaceKHR mSurface = nullptr;

	

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

		vk::raii::Buffer mIndexBuffer = nullptr;

		vk::raii::DeviceMemory mIndexBufferMemory = nullptr;

		std::vector<vk::raii::Buffer> mUniformBuffers{};
		std::vector<vk::raii::DeviceMemory> mUniformBuffersMemory{};
		std::vector<void *> mUniformBuffersMapped{};

		vk::raii::DescriptorPool mDescriptorPool = nullptr;
		std::vector<vk::raii::DescriptorSet> mDescriptorSets{};

		Ref<class Texture2D> mTexture;
		Ref<class VertexBuffer> mVertexBuffer;

		static inline GraphicsContext *sInstance = nullptr;

		Ref<VulkanSwapChain> mSwapChain;
	};
} // namespace BHive