#pragma once

#include "Core/Core.h"
#include "VulkanCore.h"

namespace BHive
{
	struct VulkanSwapChainCreateInfo
	{
		uint32_t Width{}, Height{};
		vk::SurfaceCapabilitiesKHR Capabilities;
		std::vector<vk::SurfaceFormatKHR> Formats;
		std::vector<vk::PresentModeKHR> PresentModes;
		vk::SwapchainKHR OldSwapChain = nullptr;
	};

	class VulkanSwapChain
	{
	public:
		VulkanSwapChain();

		~VulkanSwapChain();

		void Init(vk::raii::SurfaceKHR &surface, const VulkanSwapChainCreateInfo &create_info);

		void WaitForFence(uint32_t frame);

		vk::ResultValue<uint32_t> AquireNextImage(uint32_t frame);

		vk::Result Present(const vk::raii::CommandBuffer& buffer, uint32_t imageIndex, uint32_t frame);

		vk::Extent2D GetExtent() const { return mExtent; }

		const vk::SurfaceFormatKHR &GetFormat() const { return mImageFormat; }

		vk::raii::SwapchainKHR &operator*() { return mSwapChain; }

		vk::Image &GetImage(uint32_t index)  { return mImages[index]; };

		vk::raii::ImageView &GetImageView(uint32_t index)  { return mImageViews[index]; }

		uint32_t GetMinImageCount() const { return mMinImageCount; }

		uint32_t GetImageCount() const { return mImages.size(); }

		vk::ImageLayout &GetImageLayout(uint32_t imageIndex);

	private:
		vk::raii::Device &mDevice;

		vk::Extent2D mExtent{};

		vk::SurfaceFormatKHR mImageFormat{};

		vk::raii::SwapchainKHR mSwapChain = nullptr;

		std::vector<vk::Image> mImages{};

		std::vector<vk::raii::ImageView> mImageViews{};

		std::vector<vk::raii::Semaphore> mPresentSemaphores;

		std::vector<vk::raii::Semaphore> mRenderFinishedSemaphores;

		std::vector<vk::ImageLayout> mImageLayouts;

		std::vector<vk::raii::Fence> mInFlightFences;
		
		uint32_t mMinImageCount = 0;
	};
} // namespace BHive