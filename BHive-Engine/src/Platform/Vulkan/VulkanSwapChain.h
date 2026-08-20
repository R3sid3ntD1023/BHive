#pragma once

#include "Core/Core.h"
#include "VulkanImage.h"

namespace BHive
{
	class VulkanSwapChain
	{
	public:
		VulkanSwapChain(VkSurfaceKHR surface);

		~VulkanSwapChain();

		void Init(uint32_t w, uint32_t h);

		bool Recreate(uint32_t w, uint32_t h);

		void WaitForFence(uint32_t frame);

		vk::ResultValue<uint32_t> AquireNextImage(uint32_t frame);

		vk::Result Present(vk::CommandBuffer cmd, uint32_t imageIndex, uint32_t frame);

		vk::Extent2D GetExtent() const { return mExtent; }

		const vk::SurfaceFormatKHR &GetFormat() const { return mImageFormat; }

		vk::Format GetDepthStencilFormat() const { return mDepthFormat; };

		VulkanImage &GetImage(uint32_t index) { return mImages[index]; };

		VulkanImage &GetDepthImage() { return mDepthImage; }

		uint32_t GetMinImageCount() const { return mMinImageCount; }

		uint32_t GetImageCount() const { return (uint32_t)mImages.size(); }

		void BeginRendering(vk::CommandBuffer cmd, uint32_t imageindex, vk::ClearColorValue colorValue, vk::ClearDepthStencilValue depthValue);

		void EndRendering(vk::CommandBuffer cmd, uint32_t imageIndex);

	private:
		void CreateSwapChain(vk::raii::Device &device);

		void CreateSyncObjects(vk::raii::Device &device);

		void CreateImages(vk::raii::Device &device);

		void CreateDepthImage(vk::raii::Device &device);

		vk::Semaphore GetRenderFinishedSemaphore(uint32_t imageIndex);

		vk::Semaphore GetImageAvailableSemaphore(uint32_t frame);

		vk::Fence GetInFlightFence(uint32_t frame);

	private:
		vk::raii::SurfaceKHR mSurface = VK_NULL_HANDLE;

		vk::raii::SwapchainKHR mSwapChain = nullptr;

		std::vector<VulkanImage> mImages{};

		VulkanImage mDepthImage;

		std::vector<vk::raii::Semaphore> mPresentSemaphores; // per frame

		std::vector<vk::raii::Semaphore> mRenderFinishedSemaphores; // per image

		std::vector<vk::raii::Fence> mInFlightFences; // per frame

		// Cached Properties

		vk::SurfaceFormatKHR mImageFormat{};

		vk::PresentModeKHR mPresentMode{};

		vk::SurfaceCapabilitiesKHR mCapabilities{};

		vk::Extent2D mExtent{};

		uint32_t mMinImageCount = 0;

		vk::Format mDepthFormat = vk::Format::eUndefined;
	};
} // namespace BHive