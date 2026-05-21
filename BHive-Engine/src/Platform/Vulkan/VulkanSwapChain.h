#pragma once

#include "Core/Core.h"
#include "textures/VulkanImage.h"

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
		VulkanSwapChain();

		~VulkanSwapChain();

		void Init(vk::raii::SurfaceKHR &surface, const VulkanSwapChainCreateInfo &create_info);

		void WaitForFence(uint32_t frame);

		vk::ResultValue<uint32_t> AquireNextImage(uint32_t frame);

		vk::Result Present(const vk::raii::CommandBuffer& buffer, uint32_t imageIndex, uint32_t frame);

		vk::Extent2D GetExtent() const { return mExtent; }

		const vk::SurfaceFormatKHR &GetFormat() const { return mImageFormat; }

		vk::Format GetDepthStencilFormat() const { return mDepthFormat; };

		VulkanImage &GetImage(uint32_t index)  { return mImages[index]; };

		VulkanImage &GetDepthImage() { return mDepthImage; }

		uint32_t GetMinImageCount() const { return mMinImageCount; }

		uint32_t GetImageCount() const { return mImages.size(); }

	private:
		vk::raii::Device &mDevice;

		vk::Extent2D mExtent{};

		vk::SurfaceFormatKHR mImageFormat{};

		vk::raii::SwapchainKHR mSwapChain = nullptr;

		std::vector<VulkanImage> mImages{};

		std::vector<vk::raii::Semaphore> mPresentSemaphores; //per frame

		std::vector<vk::raii::Semaphore> mRenderFinishedSemaphores; //per image

		std::vector<vk::raii::Fence> mInFlightFences; //per frame

		VulkanImage mDepthImage;

		vk::Format mDepthFormat = vk::Format::eUndefined;
		
		uint32_t mMinImageCount = 0;

	};
} // namespace BHive