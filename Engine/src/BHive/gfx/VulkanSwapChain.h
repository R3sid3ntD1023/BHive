#pragma once

#include "Core/Core.h"
#include "VulkanCore.h"
#include "VulkanDevice.h"

namespace BHive
{
	struct VulkanSwapChainCreateInfo
	{
		uint32_t Width{}, Height{};
		vk::SurfaceCapabilitiesKHR Capabilities;
		std::vector<vk::SurfaceFormatKHR> Formats;
		std::vector<vk::PresentModeKHR> PresentModes;
	};

	class VulkanSwapChain
	{

	public:
		VulkanSwapChain() = default;

		~VulkanSwapChain();

		void Init(vk::raii::SurfaceKHR &surface, const VulkanSwapChainCreateInfo &create_info);

		uint32_t GetWidth() const { return mExtent.width; }

		uint32_t GetHeight() const { return mExtent.height; }

		const vk::SurfaceFormatKHR &GetFormat() const { return mImageFormat; }

		vk::raii::SwapchainKHR &operator*() { return mSwapChain; }

		vk::Image &GetImage(uint32_t index);

		vk::raii::ImageView &GetImageView(uint32_t index);

		vk::Extent2D GetExtent() const { return mExtent; }

		std::pair<vk::Result, uint32_t> AquireNextImage();

		vk::Result SubmitCommandBuffers(const std::vector<vk::CommandBuffer> &buffers, uint32_t imageIndex);

		void ResetCommandBuffer(const vk::raii::CommandBuffer &buffer);

		uint32_t GetCurrentFrame() const { return mCurrentFrame; }

		uint32_t GetMinImageCount() const { return mMinImageCount; }

		uint32_t GetImageCount() const { return mImages.size(); }

	private:
		vk::Extent2D mExtent{};

		vk::SurfaceFormatKHR mImageFormat{};

		vk::raii::SwapchainKHR mSwapChain = nullptr;

		std::vector<vk::Image> mImages{};

		std::vector<vk::raii::ImageView> mImageViews{};

		std::vector<vk::raii::Semaphore> mPresetCompleteSemaphores{};

		std::vector<vk::raii::Semaphore> mRenderFinishedSemaphores{};

		std::vector<vk::raii::Fence> mInFlightFences{};

		uint32_t mCurrentFrame = 0;

		uint32_t mSemaphoreIndex = 0;

		uint32_t mMinImageCount = 0;

		vk::raii::Image mDepthImage = nullptr;

		vk::raii::ImageView mDepthImageView = nullptr;

		vk::raii::DeviceMemory mDepthImageMemory = nullptr;
	};
} // namespace BHive